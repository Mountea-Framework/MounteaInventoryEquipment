// All rights reserved Dominik Pavlicek 2022.


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Net/UnrealNetwork.h"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{
	OnInventoryUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostInventoryUpdated);
	OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemAdded);
	OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemRemoved);
	OnItemUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemUpdated);
	
	SetIsReplicatedByDefault(true);
}

void UMounteaInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OnItemAdded.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemAdded);
	OnItemUpdated.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemUpdated);
	OnItemRemoved.AddUniqueDynamic(this, &UMounteaInventoryComponent::PostItemRemoved);
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

bool UMounteaInventoryComponent::AddItem_Implementation(UMounteaInventoryItemBase* NewItem, const int32 OptionalQuantity)
{
	return TryAddItem(NewItem, OptionalQuantity);
}

bool UMounteaInventoryComponent::AddItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& NewItems)
{
	bool bSatisfied = true;
	for (const auto Itr : NewItems)
	{
		if (!Execute_AddItem(this, Itr.Key, Itr.Value))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

bool UMounteaInventoryComponent::AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32 OptionalQuantity)
{
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

void UMounteaInventoryComponent::OnRep_Items()
{
	// TODO: update items Context
	// Broadcast changes
	
	OnInventoryUpdated.Broadcast();
}

bool UMounteaInventoryComponent::TryAddItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	if (GetOwner() && GetOwner()->HasAuthority() && Item)
	{
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
			return TryAddItem_NewItem(Item, OptionalQuantity);
		}
		else
		{
			FItemRetrievalFilter SearchFilter;
			SearchFilter.bSearchByItem = true;
			SearchFilter.Item = Item;
			SearchFilter.bSearchByClass = true;
			SearchFilter.Class = Item->StaticClass();
			
			UMounteaInventoryItemBase* ExistingItem = Execute_FindItem(this, SearchFilter);
			return TryAddItem_UpdateExisting(ExistingItem, Item, OptionalQuantity);
		}
	}

	//TODO Broadcast fail with message
	// Switch errors and broadcast only valid result
	return false;
}

bool UMounteaInventoryComponent::TryRemoveItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	return true;
}

bool UMounteaInventoryComponent::AddItem_Internal(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Items.Add(Item);

		OnInventoryUpdated.Broadcast();
		
		OnItemAdded.Broadcast(Item, MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::NewItemAdded);

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

	// If we didnt specify how much we adding all, if we did, never add more than possible
	const int32 PassQuantity = CalculateMaxPossibleQuantity(Existing, NewItem, OptionalQuantity);

	Existing->UpdateQuantity(PassQuantity);
	
	return AddItem_Internal(Existing);
}

void UMounteaInventoryComponent::PostInventoryUpdated()
{
}

void UMounteaInventoryComponent::PostItemAdded(UMounteaInventoryItemBase* Item, const FString& UpdateMessage)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			Item->GetItemAddedHandle().Broadcast(MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::NewItemAdded);
		}
	}
}

void UMounteaInventoryComponent::PostItemRemoved(UMounteaInventoryItemBase* Item, const FString& UpdateMessage)
{
}

void UMounteaInventoryComponent::PostItemUpdated(UMounteaInventoryItemBase* Item, const FString& UpdateMessage)
{
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
	// TODO
}
