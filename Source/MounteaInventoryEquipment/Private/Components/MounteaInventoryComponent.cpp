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

UMounteaInventoryItem_Base* UMounteaInventoryComponent::FindItem_Implementation(const FItemRetrievalFilter& SearchFilter)
{
	if (SearchFilter.IsValid())
	{
		return nullptr;
	}

	return nullptr;
}

TArray<UMounteaInventoryItem_Base*> UMounteaInventoryComponent::GetItems_Implementation(const FItemRetrievalFilter OptionalFilter)
{
	if (OptionalFilter.IsValid())
	{
		//TODO: Filtration
		return Items;
	}
	
	return Items;
}

bool UMounteaInventoryComponent::AddItem_Implementation(UMounteaInventoryItem_Base* NewItem)
{
	return TryAddItem(NewItem);
}

bool UMounteaInventoryComponent::AddItems_Implementation(TArray<UMounteaInventoryItem_Base*>& NewItems)
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

bool UMounteaInventoryComponent::AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItem_Base> ItemClass)
{
}

bool UMounteaInventoryComponent::AddItemsFromClass_Implementation(TArray<TSubclassOf<UMounteaInventoryItem_Base>>& NewItemsClasses)
{
}

bool UMounteaInventoryComponent::RemoveItem_Implementation(UMounteaInventoryItem_Base* AffectedItem)
{
	return TryRemoveItem(AffectedItem);
}

bool UMounteaInventoryComponent::RemoveItems_Implementation(TArray<UMounteaInventoryItem_Base*>& AffectedItems)
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

bool UMounteaInventoryComponent::TryAddItem(UMounteaInventoryItem_Base* Item)
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

bool UMounteaInventoryComponent::TryRemoveItem(UMounteaInventoryItem_Base* Item)
{
	
}

void UMounteaInventoryComponent::PostInventoryUpdated()
{
}

void UMounteaInventoryComponent::PostItemAdded(UMounteaInventoryItem_Base* Item, const FString& UpdateMessage)
{
}

void UMounteaInventoryComponent::PostItemRemoved(UMounteaInventoryItem_Base* Item, const FString& UpdateMessage)
{
}

void UMounteaInventoryComponent::PostItemUpdated(UMounteaInventoryItem_Base* Item, const FString& UpdateMessage)
{
}

void UMounteaInventoryComponent::ClientRefreshInventory_Implementation()
{
	// TODO
}
