// All rights reserved Dominik Pavlicek 2022.


#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

UMounteaInventoryComponent::UMounteaInventoryComponent()
{
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

bool UMounteaInventoryComponent::LoadInventoryFromDataTable_Implementation(const UDataTable* SourceTable)
{
	return true;
}

void UMounteaInventoryComponent::SaveInventory_Implementation()
{
}

UMounteaInventoryItem_Base* UMounteaInventoryComponent::GetItem_Implementation(const FGuid& ItemGUID)
{
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
	return true;
}

bool UMounteaInventoryComponent::AddItems_Implementation(TArray<UMounteaInventoryItem_Base*>& NewItems)
{
	return true;
}

void UMounteaInventoryComponent::OnRep_Items()
{
	// TODO: update items Context
	// Broadcast changes
}
