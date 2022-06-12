// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInventoryItemComponent.h"

void UActorInventoryItemComponent::SetItemDefinition(const FInventoryItemData NewItem)
{
	Item = NewItem;
}
