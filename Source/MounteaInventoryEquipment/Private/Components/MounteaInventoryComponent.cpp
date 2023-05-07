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

bool UMounteaInventoryComponent::HasItem_Implementation(const FItemRetrievalFilter& SearchFilter)
{
	if (SearchFilter.IsValid())
	{
		// Search by Item
		if (SearchFilter.bSearchByItem)
		{
			return Items.Contains(SearchFilter.Item);
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
				if (Itr && Itr->ItemData.ItemGuid == SearchFilter.Guid)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

UMounteaInventoryItemBase* UMounteaInventoryComponent::FindItem_Implementation(const FItemRetrievalFilter& SearchFilter)
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
				if (Itr && Itr->ItemData.ItemGuid == SearchFilter.Guid)
				{
					return Itr;
				}
			}
		}
	}

	return nullptr;
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryComponent::GetItems_Implementation(const FItemRetrievalFilter OptionalFilter)
{
	if (OptionalFilter.IsValid())
	{
		//TODO: Filtration
		return Items;
	}
	
	return Items;
}

bool UMounteaInventoryComponent::AddItem_Implementation(UMounteaInventoryItemBase* NewItem)
{
	return TryAddItem(NewItem);
}

bool UMounteaInventoryComponent::AddItems_Implementation(TArray<UMounteaInventoryItemBase*>& NewItems)
{
	bool bSatisfied = true;
	for (const auto Itr : NewItems)
	{
		if (!Execute_AddItem(this, Itr))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

bool UMounteaInventoryComponent::AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass)
{
	UMounteaInventoryItemBase* NewItem = NewObject<UMounteaInventoryItemBase>(GetWorld(), ItemClass);
	NewItem->SetWorld(GetWorld());
	return TryAddItem(NewItem);
}

bool UMounteaInventoryComponent::AddItemsFromClass_Implementation(TArray<TSubclassOf<UMounteaInventoryItemBase>>& NewItemsClasses)
{
	bool bSatisfied = true;
	for (const auto Itr : NewItemsClasses)
	{
		if (!Execute_AddItemFromClass(this, Itr))
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
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Items"))
}

bool UMounteaInventoryComponent::TryAddItem(UMounteaInventoryItemBase* Item)
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

bool UMounteaInventoryComponent::TryRemoveItem(UMounteaInventoryItemBase* Item)
{
	return true;
}

bool UMounteaInventoryComponent::TryAddItem_Internal(UMounteaInventoryItemBase* Item)
{
	if (GetOwner() && GetOwner()->HasAuthority() && Item)
	{
		FItemRetrievalFilter Filter;
		Filter.bSearchByItem = true;
		Filter.Item = Item;
		Filter.bSearchByClass = true;
		Filter.Class = Item->StaticClass();
		Filter.bSearchByTag = true;
		Filter.Tag = Item->GetFirstTag();
	
		if (!HasItem(Filter))
		{
			return TryAddItem_InternalNewItem(Item);
		}
		else
		{
			return TryAddItem_InternalUpdateExisting(Item);
		}
	}

	//TODO Broadcast fail with message
	return false;
}

bool UMounteaInventoryComponent::TryAddItem_InternalNewItem(UMounteaInventoryItemBase* Item)
{
	
	return false;
}

bool UMounteaInventoryComponent::TryAddItem_InternalUpdateExisting(UMounteaInventoryItemBase* Item)
{
	return false;
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

void UMounteaInventoryComponent::ClientRefreshInventory_Implementation()
{
	// TODO
}
