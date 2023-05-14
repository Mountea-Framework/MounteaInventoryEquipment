// All rights reserved Dominik Pavlicek 2022.


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryComponent"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{	
	SetIsReplicatedByDefault(true);
}

void UMounteaInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OnInventoryUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostInventoryUpdated);
	
	OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemAdded);
	OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemRemoved);
	OnItemUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemUpdated);
}

void UMounteaInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaInventoryComponent, Items);
}

bool UMounteaInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bUpdated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	//Check if the array of items needs to replicate
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		for (const auto& Item : Items)
		{
			if (Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
			{
				bUpdated |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}
	}

	return bUpdated;
}

TSubclassOf<UUserWidget> UMounteaInventoryComponent::GetInventoryWBPClass_Implementation()
{
	return InventoryWBPClass;
}

UUserWidget* UMounteaInventoryComponent::GetInventoryWBP_Implementation()
{
	return InventoryWBP;
}

bool UMounteaInventoryComponent::LoadInventoryFromDataTable_Implementation(const UDataTable* SourceTable)
{
	return true;
}

void UMounteaInventoryComponent::SaveInventory_Implementation()
{
	// TODO
}

bool UMounteaInventoryComponent::HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	if (SearchFilter.IsValid())
	{
		// Search by Item
		if (SearchFilter.bSearchByItem)
		{
			if (Items.Contains(SearchFilter.Item))
			{
				return true;
			}
		}

		// Search by Class
		if (SearchFilter.bSearchByClass)
		{
			for (const auto Itr : Items)
			{
				if (Itr && Itr->IsA(SearchFilter.Class))
				{
					return true;
				}
			}
		}

		// Search by Tag
		if (SearchFilter.bSearchByTag)
		{
			for (const auto Itr : Items)
			{
				if (Itr && Itr->ItemData.CompatibleGameplayTags.HasTag(SearchFilter.Tag))
				{
					return true;
				}
			}
		}

		// Search by GUID
		if (SearchFilter.bSearchByGUID)
		{
			for (const auto Itr : Items)
			{
				if (Itr && Itr->GetItemGuid() == SearchFilter.Guid)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

UMounteaInventoryItemBase* UMounteaInventoryComponent::FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) const
{
	if (SearchFilter.IsValid())
	{
		// Search by Item
		if (SearchFilter.bSearchByItem)
		{
			if (Items.Contains(SearchFilter.Item))
			{
				return Items[Items.Find(SearchFilter.Item)];
			}
		}

		// Search by Class
		if (SearchFilter.bSearchByClass)
		{
			for (const auto Itr : Items)
			{
				if (Itr && Itr->IsA(SearchFilter.Class))
				{
					return Itr;
				}
			}
		}

		// Search by Tag
		if (SearchFilter.bSearchByTag)
		{
			for (const auto Itr : Items)
			{
				if (Itr && Itr->ItemData.CompatibleGameplayTags.HasTag(SearchFilter.Tag))
				{
					return Itr;
				}
			}
		}

		// Search by GUID
		if (SearchFilter.bSearchByGUID)
		{
			for (const auto Itr : Items)
			{
				if (Itr && Itr->GetItemGuid() == SearchFilter.Guid)
				{
					return Itr;
				}
			}
		}
	}

	return nullptr;
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryComponent::GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) const
{
	if (OptionalFilter.IsValid())
	{
		TArray<UMounteaInventoryItemBase*> ReturnValues;

		//Filter by Item
		if (OptionalFilter.bSearchByItem)
		{
			if (Items.Contains(OptionalFilter.Item))
			{
				ReturnValues.Add(Items[Items.Find(OptionalFilter.Item)]);
			}
		}
		
		//Filter by Class
		if (OptionalFilter.bSearchByClass)
		{
			for (auto Itr : Items)
			{
				if (Itr && Itr->IsA(OptionalFilter.Class))
				{
					ReturnValues.Add(Itr);
				}
			}
		}

		//Filter by Tag
		if (OptionalFilter.bSearchByTag)
		{
			for (auto Itr : Items)
			{
				if (Itr && Itr->ItemData.CompatibleGameplayTags.HasTag(OptionalFilter.Tag))
				{
					ReturnValues.Add(Itr);
				}
			}
		}

		//Filter by Guid
		if (OptionalFilter.bSearchByGUID)
		{
			for (auto Itr : Items)
			{
				if (Itr && Itr->GetItemGuid() == OptionalFilter.Guid)
				{
					ReturnValues.Add(Itr);
				}
			}
		}
		return ReturnValues;
	}
	
	return Items;
}

bool UMounteaInventoryComponent::AddOrUpdateItem_Implementation(UMounteaInventoryItemBase* NewItem, const int32 OptionalQuantity)
{
	return TryAddItem(NewItem, OptionalQuantity);
}

bool UMounteaInventoryComponent::AddItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& NewItems)
{
	bool bSatisfied = true;
	for (const auto Itr : NewItems)
	{
		if (!Execute_AddOrUpdateItem(this, Itr.Key, Itr.Value))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

bool UMounteaInventoryComponent::AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32 OptionalQuantity)
{
	if (ItemClass == nullptr) return false;

	UMounteaInventoryItemBase* NewItem = NewObject<UMounteaInventoryItemBase>(GetWorld(), ItemClass);
	NewItem->SetWorld(GetWorld());
	
	return TryAddItem(NewItem, OptionalQuantity);
}

bool UMounteaInventoryComponent::AddItemsFromClass_Implementation(TMap<TSubclassOf<UMounteaInventoryItemBase>, int32>& NewItemsClasses)
{
	bool bSatisfied = true;
	for (const auto Itr : NewItemsClasses)
	{
		if (!Execute_AddItemFromClass(this, Itr.Key, Itr.Value))
		{
			bSatisfied = false;
		}
	}

	return bSatisfied;
}

bool UMounteaInventoryComponent::RemoveItem_Implementation(UMounteaInventoryItemBase* AffectedItem)
{
	return TryRemoveItem(AffectedItem);
}

bool UMounteaInventoryComponent::RemoveItems_Implementation(TArray<UMounteaInventoryItemBase*>& AffectedItems)
{
	bool bSatisfied = true;
	for (const auto Itr : AffectedItems)
	{
		if (!Execute_RemoveItem(this, Itr))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

void UMounteaInventoryComponent::RequestNetworkRefresh_Implementation()
{
	ReplicatedItemsKey++;
}

AActor* UMounteaInventoryComponent::GetOwningActor_Implementation() const
{
	return GetOwner();
}

void UMounteaInventoryComponent::OnRep_Items()
{
	// TODO: update items Context
	// Broadcast changes

	FInventoryUpdateResult UpdateResult;
	UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
	
	OnInventoryUpdated.Broadcast(UpdateResult);
}

bool UMounteaInventoryComponent::TryAddItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();

	if (!Settings)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("MounteaInventoryComponent_FailedSettings", "Failed - No Inventory Settings");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	if (!Item)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		const FText ReturnMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		const FText InvalidItem = *Settings->ItemUpdateMessages.Find(EItemUpdateResult::EIC_Failed_InvalidItem);

		UpdateResult.UpdateMessage = FText::Format(LOCTEXT("MounteaInventoryComponent_FailedItem", "{0}: {1}"), ReturnMessage, InvalidItem);
	
		OnInventoryUpdated.Broadcast(UpdateResult);
		return false;
	}
	
	if (!GetOwner())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByItem = true;
		Filter.Item = Item;
		Filter.bSearchByClass = true;
		Filter.Class = Item->GetClass();
		Filter.bSearchByTag = true;
		Filter.Tag = Item->GetFirstTag();
	}

	bool bSuccess = false;
	if (!Execute_HasItem(this, Filter))
	{
		bSuccess = TryAddItem_NewItem(Item, OptionalQuantity);
	}
	else
	{
		FItemRetrievalFilter SearchFilter;
		SearchFilter.bSearchByItem = true;
		SearchFilter.Item = Item;
		SearchFilter.bSearchByClass = true;
		SearchFilter.Class = Item->StaticClass();
			
		UMounteaInventoryItemBase* ExistingItem = Execute_FindItem(this, SearchFilter);
		
		bSuccess = TryAddItem_UpdateExisting(ExistingItem, Item, OptionalQuantity);
	}
	
	return bSuccess;
}

bool UMounteaInventoryComponent::TryRemoveItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();

	if (!Settings)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = LOCTEXT("MounteaInventoryComponent_FailedSettings", "Failed - No Inventory Settings");
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	if (!Item)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		const FText ReturnMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		const FText InvalidItem = *Settings->ItemUpdateMessages.Find(EItemUpdateResult::EIC_Failed_InvalidItem);

		UpdateResult.UpdateMessage = FText::Format(LOCTEXT("MounteaInventoryComponent_FailedItem", "{0}: {1}"), ReturnMessage, InvalidItem);
	
		OnInventoryUpdated.Broadcast(UpdateResult);
		return false;
	}
	
	if (!GetOwner())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}
	
	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByItem = true;
		Filter.Item = Item;
		Filter.bSearchByClass = true;
		Filter.Class = Item->GetClass();
		Filter.bSearchByTag = true;
		Filter.Tag = Item->GetFirstTag();
	}
	
	if (!Execute_HasItem(this, Filter))
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);
		OnInventoryUpdated.Broadcast(UpdateResult);
		
		return false;
	}

	return RemoveItem_Internal(Item, OptionalQuantity);
}

bool UMounteaInventoryComponent::AddItem_Internal(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Items.Add(Item);
		
		const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);

		FItemUpdateResult ItemUpdatedResult;
		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIC_Success_AddItem;
		ItemUpdatedResult.UpdateMessage = *Settings->ItemUpdateMessages.Find(ItemUpdatedResult.ItemUpdateResult);
		
		OnItemAdded.Broadcast(Item, ItemUpdatedResult);
		ReplicatedItemsKey++;
		OnRep_Items();
		
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::UpdateItem_Internal(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
		UpdateResult.UpdateMessage = *Settings->InventoryUpdateMessages.Find(UpdateResult.InventoryUpdateResult);

		FItemUpdateResult ItemUpdatedResult;
		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIC_Success_UpdateItem;
		ItemUpdatedResult.UpdateMessage = *Settings->ItemUpdateMessages.Find(ItemUpdatedResult.ItemUpdateResult);

		OnItemUpdated.Broadcast(Item, ItemUpdatedResult);
		ReplicatedItemsKey++;
		OnRep_Items();
		
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::RemoveItem_Internal(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	const UMounteaInventoryEquipmentSettings* const Settings = GetDefault<UMounteaInventoryEquipmentSettings>();

	// Sanity check
	if (!Item)
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Failed;
		UpdateResult.UpdateMessage = *Settings->ItemUpdateMessages.Find(EItemUpdateResult::EIC_Failed_InvalidItem);

		OnInventoryUpdated.Broadcast(UpdateResult);
		return false;
	}
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FItemUpdateResult ItemUpdatedResult;
		
		int32 AmountToRemove = -1;
		if (OptionalQuantity == 0)
		{
			AmountToRemove = Item->ItemData.ItemQuantity.CurrentQuantity;
		}
		else
		{
			AmountToRemove = OptionalQuantity;
		}

		AmountToRemove = FMath::Abs(AmountToRemove);
		const int32 MaxToRemove = Item->ItemData.ItemQuantity.CurrentQuantity;
		AmountToRemove =  FMath::Abs(FMath::Min(AmountToRemove, MaxToRemove));

		AmountToRemove = MaxToRemove - AmountToRemove;
		
		Item->UpdateQuantity(AmountToRemove);
		
		if (Item->ItemData.ItemQuantity.CurrentQuantity <= 0)
		{
			ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIC_Success_RemovedItem;
			ItemUpdatedResult.UpdateMessage = *Settings->ItemUpdateMessages.Find(ItemUpdatedResult.ItemUpdateResult);
			
			OnItemRemoved.Broadcast(Item, ItemUpdatedResult);
			Items.Remove(Item);

			ReplicatedItemsKey++;
			OnRep_Items();

			return true;
		}

		ItemUpdatedResult.ItemUpdateResult = EItemUpdateResult::EIC_Success_UpdateItem;
		ItemUpdatedResult.UpdateMessage = *Settings->ItemUpdateMessages.Find(ItemUpdatedResult.ItemUpdateResult);

		OnItemUpdated.Broadcast(Item, ItemUpdatedResult);
		ReplicatedItemsKey++;
		OnRep_Items();
		
		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::TryAddItem_NewItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	// Sanity check
	if (!Item) return false;

	if (OptionalQuantity < 0)
	{
		return TryRemoveItem(Item, OptionalQuantity);
	}
	
	const int32 FinalQuantity = CalculateMaxPossibleQuantity(Item, nullptr, OptionalQuantity);
	Item->UpdateQuantity(FinalQuantity);
	
	// In this case we actually can just add the item with no further checks
	return AddItem_Internal(Item);
}

bool UMounteaInventoryComponent::TryAddItem_UpdateExisting(UMounteaInventoryItemBase* Existing, UMounteaInventoryItemBase* NewItem, const int32 OptionalQuantity)
{
	// Sanity check
	if (!NewItem) return false;
	if (!Existing) return false;

	if (OptionalQuantity < 0)
	{
		return TryRemoveItem(Existing, OptionalQuantity);
	}

	// If we didnt specify how much we adding all, if we did, never add more than possible
	const int32 PassQuantity = CalculateMaxPossibleQuantity(Existing, NewItem, OptionalQuantity);

	Existing->UpdateQuantity(PassQuantity);
	
	return UpdateItem_Internal(Existing);
}

void UMounteaInventoryComponent::PostInventoryUpdated_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	PostInventoryUpdated_Client(UpdateContext);
}

void UMounteaInventoryComponent::PostInventoryUpdated_Client_Implementation(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;

	if (GetOwner()->HasAuthority()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestSyncTimerHandle);

	FTimerDelegate TimerDelegate_RequestSyncTimerHandle;
	TimerDelegate_RequestSyncTimerHandle.BindUFunction(this, "PostInventoryUpdated_Client_RequestUpdate", UpdateContext);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RequestSyncTimerHandle, TimerDelegate_RequestSyncTimerHandle, Duration_RequestSyncTimerHandle, false);
}

void UMounteaInventoryComponent::PostInventoryUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext)
{
	if (!GetOwner()) return;
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RequestSyncTimerHandle);

	OnInventoryUpdated_Client.Broadcast(UpdateContext);
}

void UMounteaInventoryComponent::PostItemAdded(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			Item->GetItemAddedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful);
		}

		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
		OnInventoryUpdated.Broadcast(UpdateResult);
	}
}

void UMounteaInventoryComponent::PostItemRemoved(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
		OnInventoryUpdated.Broadcast(UpdateResult);
	}
}

void UMounteaInventoryComponent::PostItemUpdated(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			Item->GetItemUpdatedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::UpdateSuccessful);
		}
		
		FInventoryUpdateResult UpdateResult;
		UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
		OnInventoryUpdated.Broadcast(UpdateResult);
	}
}

int32 UMounteaInventoryComponent::CalculateMaxPossibleQuantity(const UMounteaInventoryItemBase* Item, const UMounteaInventoryItemBase* OtherItem, const int32 OptionalQuantity) const
{
	if (!Item)
	{
		return 0;
	}
	
	if (OtherItem)
	{
		// Updating item that cannot stack mean always just and only 0
		if (!Item->ItemData.ItemQuantity.bIsStackable)
		{
			return 0;
		}
		
		const int32 UpdateQuantity = OptionalQuantity != 0 ? OptionalQuantity : OtherItem->ItemData.ItemQuantity.CurrentQuantity;
	
		const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
		const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;

		const int32 MaxPossible = MaxQuantity - CurrentQuantity;
	
		const int32 PassQuantity = FMath::Min(MaxPossible, CurrentQuantity + UpdateQuantity);

		return PassQuantity;
	}
	else
	{
		// Adding new item that cannot stack mean always just and only 1
		if (!Item->ItemData.ItemQuantity.bIsStackable)
		{
			return 1;
		}

		const int32 UpdateQuantity = OptionalQuantity != 0 ? OptionalQuantity : Item->ItemData.ItemQuantity.CurrentQuantity;
	
		const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
		const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;

		const int32 MaxPossible = MaxQuantity - CurrentQuantity;
	
		const int32 PassQuantity = FMath::Min(MaxPossible, UpdateQuantity);

		return PassQuantity;
	}
	
	return 0;
}

void UMounteaInventoryComponent::ClientRefreshInventory_Implementation()
{
	ReplicatedItemsKey++;

	FInventoryUpdateResult UpdateResult;
	UpdateResult.InventoryUpdateResult = EInventoryUpdateResult::EIC_Success;
	OnInventoryUpdated.Broadcast(UpdateResult);
}

#undef LOCTEXT_NAMESPACE