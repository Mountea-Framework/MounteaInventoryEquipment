// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/MounteaInventoryItem.h"

#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"


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
}

void UMounteaInventoryItemBase::UpdateQuantity(const int32 NewQuantity)
{
	ItemData.ItemQuantity.CurrentQuantity = FMath::Min(ItemData.ItemQuantity.MaxQuantity, NewQuantity);

	OnItemModified.Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::ItemNotifications::ItemUpdated);
}

FGameplayTag UMounteaInventoryItemBase::GetFirstTag() const
{
	return ItemData.CompatibleGameplayTags.First();
}

void UMounteaInventoryItemBase::SetWorld(UWorld* NewWorld)
{
	World = NewWorld;
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

void UMounteaInventoryItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryItemBase, ItemData);
}

bool UMounteaInventoryItemBase::IsSupportedForNetworking() const
{
	return true;
}

UWorld* UMounteaInventoryItemBase::GetWorld() const
{
	if (World)
	{
		return World;
	}
	
	if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	
	return nullptr;
}

void UMounteaInventoryItemBase::MarkDirtyForReplication()
{
	RepKey++;
	
	if (OwningInventory.GetInterface())
	{
		OwningInventory->Execute_RequestNetworkRefresh(OwningInventory.GetObject());
	}
}

#if WITH_EDITOR
void UMounteaInventoryItemBase::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);

	ItemGuid = FGuid::NewGuid();
}

void UMounteaInventoryItemBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// ...
}
#endif