// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "Decorations/MounteaSelectableInventoryItemAction.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Statics/MounteaInventoryStatics.h"


bool UMounteaSelectableInventoryItemAction::IsAllowed_Implementation(const UMounteaJsonObject* Payload) const
{
	if (!IsActionVisible(Payload))
		return false;

	UObject* inventory = nullptr;
	FMounteaInventoryItem item;
	if (!ResolveItemFromPayload(Payload, inventory, item) || !IsValid(item.GetTemplate()))
		return false;
	
	return item.GetTemplate()->ItemActions.ContainsByPredicate([this](const UMounteaSelectableInventoryItemAction* action)
	{
		return IsValid(action) && action->GetClass() == GetClass();
	});
}

void UMounteaSelectableInventoryItemAction::ExecuteQueuedAction_Implementation(UObject* OptionalPayload)
{
	OnSelectableActionQueueExecuted.Broadcast(this, OptionalPayload);
}

bool UMounteaSelectableInventoryItemAction::IsActionVisible(const UMounteaJsonObject* Payload) const
{
	return true;
}



