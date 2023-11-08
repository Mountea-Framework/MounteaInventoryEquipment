// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaInventoryInstancedItem.h"

#include "ObjectTrace.h"
#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Net/UnrealNetwork.h"

UMounteaInstancedItem::UMounteaInstancedItem()
{
	RepKey = 0;
}

UMounteaInstancedItem::~UMounteaInstancedItem()
{
	TRACE_OBJECT_LIFETIME_END(this);
}

bool UMounteaInstancedItem::InitializeNewItem_Implementation(const FItemInitParams& InitParams)
{
	if (!InitParams.Outer)
	{
		return false;
	}

	if (InitParams.OwningInventory.GetObject() == nullptr)
	{
		return false;
	}

	if (!InitParams.Outer->GetWorld())
	{
		return false;
	}

	if (InitParams.Quantity <= 0)
	{
		return false;
	}

	SetWorld(InitParams.Outer->GetWorld());
	OwningInventory = InitParams.OwningInventory;

	if (InitParams.SourceItem)
	{
		ItemDataSource = EItemDataSource::EIDS_SourceItem;
		SetSourceItem(InitParams.SourceItem);
	}
	else if (InitParams.SourceTable && InitParams.SourceRow.IsValid())
	{
		FMounteaInventoryItemData* Row = GetRow<FMounteaInventoryItemData>(InitParams.SourceRow, InitParams.SourceTable);
		
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
		OnItemBeginPlay("[UMounteaInstancedItem::SetValidData] Begin Play Called Successfully");

		TRACE_OBJECT_LIFETIME_BEGIN(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UMounteaInstancedItem::SetValidData] Cannot initialize Item, will be deleted"))
		
		DestroyItem();
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

	bool bItemConfigFound = false;
	ItemConfig.ItemConfig = SourceItem->GetItemConfig(SourceItem->GetItemConfigClass(), bItemConfigFound);

	ItemFlags = SourceItem->GetItemData().ItemFlags;
	
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

	ItemConfig.ItemConfig = ItemData->ItemConfig;
	
	ItemFlags = ItemData->RequiredData.ItemFlags;
	return true;
}

void UMounteaInstancedItem::CleanupData()
{
	Quantity = 0;

	ItemConfig.ItemConfig = nullptr;
	ItemFlags.Reset();
}

void UMounteaInstancedItem::UpdateQuantity(const int32& NewValue)
{
	Quantity = FMath::Max(0, NewValue);

	if (NewValue <= 0)
	{
		DestroyItem();
	}
}

void UMounteaInstancedItem::DestroyItem()
{
	//TODO
	// Destroy Stacks
	// Call to UI to remove them all
	// Mark garbage
}

void UMounteaInstancedItem::AddItemFlag(const FGameplayTag& NewFlag)
{
	ItemFlags.AddTag(NewFlag);
}

void UMounteaInstancedItem::SetItemFlags(const FGameplayTagContainer& NewFlags)
{
	ItemFlags = NewFlags;
}

void UMounteaInstancedItem::RemoveItemFlag(const FGameplayTag& RemoveFlag)
{
	if (ItemFlags.HasTag(RemoveFlag))
	{
		ItemFlags.RemoveTag(RemoveFlag);
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
}

bool UMounteaInstancedItem::OwnerHasAuthority() const
{
	if (!OwningInventory)
	{
		if (!GEngine) return false;
		if (!World) return false;

		const FString ModeName = GetEnumValueAsString(TEXT("ENetRole"), GEngine->GetNetMode(World));

		UE_LOG(LogTemp, Error, TEXT("%s"), *ModeName)
		if (GEngine->GetNetMode(World) == NM_Client)
		{
			return false;
		}

		return true;
	}

	return OwningInventory->Execute_DoesHaveAuthority(OwningInventory.GetObject());
}

void UMounteaInstancedItem::MarkDirtyForReplication()
{
	RepKey++;
	
	if (OwningInventory.GetInterface())
	{
		OwningInventory->Execute_RequestNetworkRefresh(OwningInventory.GetObject());
	}
}

void UMounteaInstancedItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInstancedItem, ItemDataSource);
	DOREPLIFETIME(UMounteaInstancedItem, SourceItem);
	DOREPLIFETIME(UMounteaInstancedItem, SourceTable);
	DOREPLIFETIME(UMounteaInstancedItem, SourceRow);
	DOREPLIFETIME(UMounteaInstancedItem, OwningInventory);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, World, COND_InitialOrOwner);
	DOREPLIFETIME_CONDITION(UMounteaInstancedItem, InstanceID, COND_InitialOnly);
	DOREPLIFETIME(UMounteaInstancedItem, ItemConfig);
	DOREPLIFETIME(UMounteaInstancedItem, Quantity);
	DOREPLIFETIME(UMounteaInstancedItem, ItemFlags);
}

void UMounteaInstancedItem::OnRep_Item()
{
	// TODO
}

void UMounteaInstancedItem::OnRep_Quantity()
{
	// TODO:
	// Calculate if there is 0
	// If so, DestroyItem
}
