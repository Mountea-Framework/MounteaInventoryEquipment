// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaInventoryItem.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"

#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/MounteaInventoryEquipmentBPF.h"

UMounteaInventoryItemBase::UMounteaInventoryItemBase()
{
	RepKey = 0;
}

void UMounteaInventoryItemBase::PostInitProperties()
{
	UObject::PostInitProperties();

	bool bIsEditorNoPlay = false;
#if WITH_EDITOR
	bIsEditorNoPlay = UMounteaInventoryEquipmentBPF::IsEditorNoPlay();
#endif
	
	if (bIsEditorNoPlay) // This code gets executed only when opening new Asset in Editor
	{
		if (ItemDataSource == EItemDataSource::EIDS_SourceTable && SourceTable == nullptr)
		{
			SourceTable = UMounteaInventoryEquipmentBPF::GetDefaultItemsTable();
		}

		EnsureValidConfig();
	}
	else
	{
		OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemAdded);
		OnItemInitialized.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemInitialized);
		OnItemModified.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemModified);
		OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemRemoved);
		
		SetValidData();

		OnItemBeginPlay(TEXT("Item has been initialized"));
	}
}

bool UMounteaInventoryItemBase::IsValid(UObject* WorldContextObject) const
{
	return GetWorld() ? true : false;
}

void UMounteaInventoryItemBase::AddQuantity(const int32 Amount)
{
	const int32 Current = ItemData.ItemQuantity.CurrentQuantity;
	const int32 Requested = Current + Amount;

	if (Current == Requested) return;
	
	const int32 Max = ItemData.ItemQuantity.bIsStackable ? ItemData.ItemQuantity.MaxQuantity : 1;
	
	ItemData.ItemQuantity.CurrentQuantity = FMath::Min(Requested, Max);

	OnItemModified.Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::ItemNotifications::ItemUpdated);
}

void UMounteaInventoryItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity == ItemData.ItemQuantity.CurrentQuantity) return;
	
	ItemData.ItemQuantity.CurrentQuantity = FMath::Min(ItemData.ItemQuantity.MaxQuantity, NewQuantity);

	OnItemModified.Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::ItemNotifications::ItemUpdated);
}

int32 UMounteaInventoryItemBase::GetQuantity() const
{
	return ItemData.ItemQuantity.CurrentQuantity;
}

void UMounteaInventoryItemBase::SetWorldFromLevel(ULevel* FromLevel)
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

void UMounteaInventoryItemBase::SetWorld(UWorld* NewWorld)
{
	World = NewWorld;
}

void UMounteaInventoryItemBase::InitializeNewItem(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory)
{
	if (NewOwningInventory.GetObject() == nullptr)
	{
		return;
	}

	if (const AActor* InventoryOwner = NewOwningInventory->Execute_GetOwningActor(NewOwningInventory.GetObject()))
	{
		World = InventoryOwner->GetWorld();
		OwningInventory = NewOwningInventory;
	
		MarkDirtyForReplication();

		FString Message = ItemData.ItemName.ToString();
		Message.Append(": Initialization completed.");
		OnItemInitialized.Broadcast(Message);
	}
}

void UMounteaInventoryItemBase::OnRep_Item()
{
	FString Message = ItemData.ItemName.ToString();
	Message.Append(" has been modified.");
	
	OnItemModified.Broadcast(Message);
}

void UMounteaInventoryItemBase::OnItemBeginPlay_Implementation(const FString& Message)
{
	OnItemInitialized.Broadcast(Message);
}

void UMounteaInventoryItemBase::ItemAdded(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::ItemInitialized(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::ItemModified(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::ItemRemoved(const FString& Message)
{
	MarkDirtyForReplication();
}

void UMounteaInventoryItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryItemBase, ItemData);
	DOREPLIFETIME(UMounteaInventoryItemBase, ItemDataSource);
}

bool UMounteaInventoryItemBase::IsSupportedForNetworking() const
{
	return true;
}

void UMounteaInventoryItemBase::MarkDirtyForReplication()
{
	RepKey++;
	
	if (OwningInventory.GetInterface())
	{
		OwningInventory->Execute_RequestNetworkRefresh(OwningInventory.GetObject());
	}
}

void UMounteaInventoryItemBase::ClearDataTable()
{
	SourceTable = nullptr;
	SourceRow = FName();
}

void UMounteaInventoryItemBase::CopyFromTable()
{
	if (SourceTable == nullptr)
	{
		ClearDataTable();
		return;
	}
	
	FString Context;
	if (const FMounteaInventoryItemData* Row = GetRow<FMounteaInventoryItemData>(SourceRow, SourceTable))
	{
		ItemData = Row->RequiredData;
		ItemOptionalData = Row->OptionalData;
	}
	else
	{
		//TODO: Show error that no valid data found
		
		ItemData = FMounteaInventoryItemRequiredData();
		ItemOptionalData = FMounteaInventoryItemOptionalData();
	}
}

void UMounteaInventoryItemBase::SetValidData()
{
	switch (ItemDataSource)
	{
		case EItemDataSource::EIDS_SourceTable:
			ClearMappedValues();
			CopyFromTable();
			CopyTagsFromTypes();
			break;
		case EItemDataSource::EIDS_ManualInput:
			ClearDataTable();
			CopyTagsFromTypes();
			break;
		case EItemDataSource::Default:
		default: break;
	}
}

void UMounteaInventoryItemBase::SetValidDataEditor()
{
	SetValidData();
	EnsureValidConfig();
}

void UMounteaInventoryItemBase::ClearMappedValues()
{
	ItemData = FMounteaInventoryItemRequiredData();
	ItemOptionalData = FMounteaInventoryItemOptionalData();
}

void UMounteaInventoryItemBase::CopyTagsFromTypes()
{
	if (ItemData.ItemCategory)
	{
		ItemData.CompatibleGameplayTags.AppendTags(ItemData.ItemCategory->CompatibleGameplayTags);
	}

	if (ItemData.ItemRarity)
	{
		ItemData.CompatibleGameplayTags.AddTag(ItemData.ItemRarity->RarityGameplayTag);
	}
}

#if WITH_EDITOR

void UMounteaInventoryItemBase::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);

	ItemGuid = FGuid::NewGuid();
}

void UMounteaInventoryItemBase::EnsureValidConfig()
{
	if (ItemConfig.ItemConfig == nullptr)
	{
		bool bFound = false;
		const TSubclassOf<UMounteaInventoryItemConfig> Class = UMounteaInventoryEquipmentBPF::GetSettings()->DefaultItemConfigClass.LoadSynchronous();
		ItemConfig.ItemConfig = UMounteaInventoryEquipmentBPF::GetItemConfig(this, Class, bFound);
	}
}

void UMounteaInventoryItemBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("bIsStackable"))
	{
		if (!ItemData.ItemQuantity.bIsStackable)
		{
			ItemData.ItemQuantity.MaxQuantity = 1;
		}
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, ItemDataSource))
	{
		SetValidData();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, SourceTable))
	{
		SourceRow = FName();
		SetValidData();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, SourceRow))
	{
		SetValidData();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, ItemConfig))
	{
		EnsureValidConfig();
	}
}
#endif