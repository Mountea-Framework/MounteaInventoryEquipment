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
		return true;
	}

	return false;
}

UMounteaInventoryItemBase* UMounteaInventoryComponent::FindItem_Implementation(const FItemRetrievalFilter& SearchFilter)
{
	if (SearchFilter.IsValid())
	{
		return nullptr;
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
		if (!AddItem_Implementation(Itr))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

bool UMounteaInventoryComponent::AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass)
{
	return true;
}

bool UMounteaInventoryComponent::AddItemsFromClass_Implementation(TArray<TSubclassOf<UMounteaInventoryItemBase>>& NewItemsClasses)
{
	bool bSatisfied = true;
	for (const auto Itr : NewItemsClasses)
	{
		if (!AddItemFromClass_Implementation(Itr))
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
		if (!RemoveItem_Implementation(Itr))
		{
			bSatisfied = false;
		}
	}
	
	return bSatisfied;
}

void UMounteaInventoryComponent::OnRep_Items()
{
	// TODO: update items Context
	// Broadcast changes
}

bool UMounteaInventoryComponent::TryAddItem(UMounteaInventoryItemBase* Item)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Items.Add(Item);

		OnInventoryUpdated.Broadcast();
		
		OnItemAdded.Broadcast(Item, MounteaInventoryEquipmentConsts::MounteaInventoryNotifications::InventoryNotifications::NewItemAdded);

		return true;
	}

	return false;
}

bool UMounteaInventoryComponent::TryRemoveItem(UMounteaInventoryItemBase* Item)
{
	return true;
}

void UMounteaInventoryComponent::PostInventoryUpdated()
{
}

void UMounteaInventoryComponent::PostItemAdded(UMounteaInventoryItemBase* Item, const FString& UpdateMessage)
{
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
