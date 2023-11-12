// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Definitions/MounteaInventoryInstancedItem.h"

#include "ObjectTrace.h"
#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Interfaces/MounteaInventoryInterface.h"

#include "Net/UnrealNetwork.h"

UMounteaInstancedItem::UMounteaInstancedItem()
{
	RepKey = 0;
}

UMounteaInstancedItem::~UMounteaInstancedItem()
{
	TRACE_OBJECT_LIFETIME_END(this);
}

void UMounteaInstancedItem::PostInitProperties()
{
	UObject::PostInitProperties();

	if (FApp::IsGame())
	{
		//...
	}
}

bool UMounteaInstancedItem::InitializeNewItem_Implementation(const FItemInitParams& InitParams)
{
	if (InitParams.OwningInventory.GetObject() == nullptr)
	{
		return false;
	}

	if (!InitParams.OwningInventory->Execute_GetOwningActor(OwningInventory.GetObject()))
	{
		return false;
	}

	if (InitParams.Quantity <= 0)
	{
		return false;
	}

	UWorld* NewWorld =
		InitParams.OwningInventory.GetObject()->GetWorld() ?
			InitParams.OwningInventory.GetObject()->GetWorld() :
			InitParams.OwningInventory->Execute_GetOwningActor(OwningInventory.GetObject())->GetWorld();
	
	// Validate nullptr!
	SetWorld(NewWorld);
	OwningInventory = InitParams.OwningInventory;

	if (InitParams.SourceItem)
	{
		ItemDataSource = EItemDataSource::EIDS_SourceItem;
		SetSourceItem(InitParams.SourceItem);
	}
	else if (InitParams.SourceTable && InitParams.SourceRow.IsValid())
	{
		const FMounteaInventoryItemData* Row = GetRow<FMounteaInventoryItemData>(InitParams.SourceRow, InitParams.SourceTable);
		
		if (Row == nullptr)
		{
			return false;
		}

		ItemDataSource = EItemDataSource::EIDS_SourceTable;
		SetSourceTable(InitParams.SourceTable, InitParams.SourceRow.ToString());
	}
	else
	{
		return false;
	}

	SetValidData();

	return true;
}

void UMounteaInstancedItem::SetValidData()
{
	if (ConstructItem())
	{
		Execute_OnItemBeginPlay(this, "[UMounteaInstancedItem::SetValidData] Begin Play Called Successfully");

		OnItemInitialized.Broadcast("Item Initialized");

		TRACE_OBJECT_LIFETIME_BEGIN(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UMounteaInstancedItem::SetValidData] Cannot initialize Item, will be deleted"))
		
		EmptyItem();
	}
}

void UMounteaInstancedItem::SetSourceItem(UMounteaInventoryItemBase* NewSourceItem)
{
	SourceItem = NewSourceItem;
}

void UMounteaInstancedItem::SetSourceTable( UMounteaInventoryItemsTable* Table, const FString& RowName)
{
	SourceTable = Table;
	SourceRow = (TEXT("%s"), *RowName);
}

bool UMounteaInstancedItem::ConstructItem()
{
	bool bSuccess = false;

	if (SourceTable == nullptr && SourceItem == nullptr)
	{
		return bSuccess;
	}
	
	// Clear existing data before constructing a new item
	CleanupData();
	
	switch (ItemDataSource)
	{
		case EItemDataSource::EIDS_SourceItem:
				bSuccess = CopyFromBaseItem();
				break;

		case EItemDataSource::EIDS_SourceTable:
				bSuccess = CopyFromDataTable();
				break;
	}

	return bSuccess;
}

bool UMounteaInstancedItem::CopyFromBaseItem()
{
	if (!SourceItem)
	{
		return false;
	}

	// Construct new Config
	bool bItemConfigFound = false;
	ItemConfig.ItemConfig = NewObject<UMounteaInventoryItemConfig>
	(
		GetPackage()->GetOuter(),
		SourceItem->GetItemConfigClass(),
		NAME_None,
		RF_NoFlags,
		SourceItem->GetItemConfig(SourceItem->GetItemConfigClass(), bItemConfigFound)
	);

	// Copy Flags (including those from Category and Rarity
	ItemFlags = SourceItem->GetItemData().RequiredData.ItemFlags;
	
	return true;
}

bool UMounteaInstancedItem::CopyFromDataTable()
{
	if (!SourceTable)
	{
		return false;
	}

	if (!SourceRow.IsValid())
	{
		return false;
	}

	const FMounteaInventoryItemData* ItemData = GetRow<FMounteaInventoryItemData>(SourceRow, SourceTable);
	if (!ItemData)
	{
		return false;
	}

	// Construct new Config
	if (ItemData->ItemConfig)
	{
		ItemConfig.ItemConfig = NewObject<UMounteaInventoryItemConfig>
		(
			GetPackage()->GetOuter(),
			ItemData->ItemConfig->StaticClass(),
			NAME_None,
			RF_NoFlags,
			ItemData->ItemConfig
		);
	}

	// Copy Flags (including those from Category and Rarity
	ItemFlags = ItemData->RequiredData.ItemFlags;
	
	return true;
}

void UMounteaInstancedItem::CleanupData()
{
	Quantity = 0;

	if (ItemConfig.ItemConfig)
	{
		ItemConfig.ItemConfig->MarkAsGarbage();
	}
	
	ItemConfig.ItemConfig = nullptr;
	ItemFlags.Reset();
}

void UMounteaInstancedItem::SetQuantity(const int32& NewValue)
{
	Quantity = FMath::Max(0, NewValue);

	if (NewValue <= 0)
	{
		EmptyItem();
	}

	MarkDirtyForReplication();
}

int32 UMounteaInstancedItem::ModifyQuantity(const int32& ValueToAdd)
{
	const int32 MaxQuantity = GetItemData().RequiredData.ItemQuantity.MaxQuantity;

	// Trying to subtract to null or negative
	if (Quantity + ValueToAdd <= 0)
	{
		EmptyItem();
		
		return 0;
	}
	
	// Trying to add over
	if (Quantity + ValueToAdd > MaxQuantity)
	{
		const int32 Remainder = (Quantity + ValueToAdd) - MaxQuantity;
		const int32 NewQuantity = MaxQuantity;

		SetQuantity(MaxQuantity);

		return Remainder;
	}
	
	return 0;
}

void UMounteaInstancedItem::EmptyItem()
{
	//TODO
	// Destroy Stacks
	// Call to UI to remove them all
	// Mark garbage

	MarkDirtyForReplication();
}

FMounteaInventoryItemData UMounteaInstancedItem::GetItemData() const
{
	switch (ItemDataSource)
	{
		case EItemDataSource::EIDS_SourceTable:
			{
				if (FMounteaInventoryItemData* const Row = GetRow<FMounteaInventoryItemData>(SourceRow, SourceTable))
				{
					return *Row;
				}
			}
			break;
		case EItemDataSource::EIDS_SourceItem:
			if (SourceItem)
			{
				bool bResult = false;
				return SourceItem->ItemData;
			}
			break;
		case EItemDataSource::Default:
		default:
			break;
	}

	return FMounteaInventoryItemData();
}

int32 UMounteaInstancedItem::GetQuantity() const
{
	return Quantity;
}

void UMounteaInstancedItem::AddItemFlag(const FGameplayTag& NewFlag)
{
	ItemFlags.AddTag(NewFlag);

	OnItemModified.Broadcast("Added new Item Flag");
	
	MarkDirtyForReplication();	
}

void UMounteaInstancedItem::SetItemFlags(const FGameplayTagContainer& NewFlags)
{
	ItemFlags = NewFlags;

	OnItemModified.Broadcast("Item Flags bulk updated");
	
	MarkDirtyForReplication();	
}

void UMounteaInstancedItem::RemoveItemFlag(const FGameplayTag& RemoveFlag)
{
	if (ItemFlags.HasTag(RemoveFlag))
	{
		ItemFlags.RemoveTag(RemoveFlag);

		OnItemModified.Broadcast("Removed Item Flag");
	
		MarkDirtyForReplication();	
	}
}

bool UMounteaInstancedItem::IsFlagSet(const FGameplayTag& QueryFlag) const
{
	return ItemFlags.HasTag(QueryFlag);
}

bool UMounteaInstancedItem::AreFlagsSet(const FGameplayTagContainer& QueryFlags, const bool bSimpleSearch) const
{
	bool bAreFlagsSet = false;

	if (bSimpleSearch)
	{
		bAreFlagsSet = ItemFlags.HasAny(QueryFlags);
	}
	else
	{
		bAreFlagsSet = ItemFlags.HasAll(QueryFlags);
	}

	return bAreFlagsSet;
}

void UMounteaInstancedItem::SetOwningInventory(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory)
{
	OwningInventory = NewOwningInventory;

	OnItemModified.Broadcast("Inventory Updated");
	
	MarkDirtyForReplication();	
}

void UMounteaInstancedItem::SetWorldFromLevel(ULevel* FromLevel)
{
	if (FromLevel == nullptr)
	{
		return;
	}
	
	if (FromLevel->GetWorld() != World)
	{
		SetWorld(FromLevel->GetWorld());
	}
}

void UMounteaInstancedItem::SetWorld(UWorld* NewWorld)
{
	if (!NewWorld) return;
	
	World = NewWorld;

	OnItemModified.Broadcast("World Updated");
	
	MarkDirtyForReplication();	
}

bool UMounteaInstancedItem::OwnerHasAuthority() const
{
	if (OwningInventory)
	{
		// If OwningInventory is valid, delegate the authority check to it.
		return OwningInventory->Execute_DoesHaveAuthority(OwningInventory.GetObject());
	}
	else
	{
		// If OwningInventory is null, use global checks.
		if (GEngine && World)
		{
			const ENetMode NetMode = GEngine->GetNetMode(World);
			UE_LOG(LogTemp, Log, TEXT("NetMode is %s"), *GetEnumValueAsString(TEXT("ENetRole"), NetMode));
			return NetMode != NM_Client;
		}
	}
    
	// If we reach this point, we don't have enough information to determine authority.
	return false;
}


void UMounteaInstancedItem::MarkDirtyForReplication()
{
	NetFlush();
	
	if (OwningInventory.GetInterface())
	{
		OwningInventory->Execute_RequestNetworkRefresh(OwningInventory.GetObject());
	}
}

void UMounteaInstancedItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UMounteaInstancedItem, OwningInventory);
	DOREPLIFETIME(UMounteaInstancedItem, Quantity);
	DOREPLIFETIME(UMounteaInstancedItem, ItemFlags);
	
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, ItemDataSource, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, SourceItem, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, SourceTable, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, SourceRow, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, World, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, InstanceID, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, ItemConfig, COND_InitialOrOwner);
}

void UMounteaInstancedItem::OnRep_Item()
{
	// TODO
	OnItemModified.Broadcast("Item Modified");
}

void UMounteaInstancedItem::OnRep_Quantity()
{
	// TODO:
	// Calculate if there is 0
	// If so, DestroyItem

	OnItemModified.Broadcast("Quantity Has Changed");
}