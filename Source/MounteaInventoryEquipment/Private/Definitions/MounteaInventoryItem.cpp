// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/MounteaInventoryItem.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"

#include "Helpers/FMounteaTemplatesLibrary.h"

UMounteaInventoryItemBase::UMounteaInventoryItemBase()
{
	RepKey = 0;
}

void UMounteaInventoryItemBase::PostInitProperties()
{
	UObject::PostInitProperties();

	OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemAdded);
	OnItemInitialized.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemInitialized);
	OnItemModified.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemModified);
	OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryItemBase::ItemRemoved);

	RefreshData();

	OnItemBeginPlay(TEXT("Item has been initialized"));
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

void UMounteaInventoryItemBase::SetWorld(UWorld* NewWorld)
{
	if (NewWorld != World)
	{
		World = NewWorld;

		FString Message = TEXT("World updated for Item ");
		Message.Append(GetName());
		
		OnItemModified.Broadcast(Message);
	}
}

void UMounteaInventoryItemBase::InitializeNewItem(UWorld* NewWorld, TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory, const FMounteaInventoryItemRequiredData& NewItemData, const FMounteaInventoryItemOptionalData NewOptionalData)
{
	World = NewWorld;
	OwningInventory = NewOwningInventory;
	ItemData = NewItemData;
	ItemOptionalData = NewOptionalData;

	MarkDirtyForReplication();

	FString Message = ItemData.ItemName.ToString();
	Message.Append(": Initialization completed.");
	OnItemInitialized.Broadcast(Message);
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
	FString Context;
	if (const FMounteaInventoryItemData* Row = GetRow<FMounteaInventoryItemData>(SourceRow, SourceTable))
	{
		ItemData = Row->RequiredData;
		ItemOptionalData = Row->OptionalData;
	}
	
	//TODO: Show error that no valid data found
}

void UMounteaInventoryItemBase::ClearMappedValues()
{
	ItemData = FMounteaInventoryItemRequiredData();
	ItemOptionalData = FMounteaInventoryItemOptionalData();
}

#if WITH_EDITOR

void UMounteaInventoryItemBase::RefreshData()
{
	if (SourceTable != nullptr)
	{
		CopyFromTable();
	}
}

void UMounteaInventoryItemBase::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);

	ItemGuid = FGuid::NewGuid();
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
		switch (ItemDataSource)
		{
			case EItemDataSource::EIDS_SourceTable:
				ClearMappedValues();
				break;
			case EItemDataSource::EIDS_ManualInput:
				ClearDataTable();
				break;
			case EItemDataSource::Default:
			default: break;
		}
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, SourceTable))
	{
		SourceRow = FName();
		ClearMappedValues();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemBase, SourceRow))
	{
		CopyFromTable();
	}
}
#endif