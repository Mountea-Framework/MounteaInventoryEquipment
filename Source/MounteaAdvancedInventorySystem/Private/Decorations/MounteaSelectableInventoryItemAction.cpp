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

#include "Statics/MounteaInventoryStatics.h"

bool UMounteaSelectableInventoryItemAction::IsAllowed_Implementation(const FMounteaInventoryItem& TargetItem) const
{
	if (!Super::IsAllowed_Implementation(TargetItem) && IsActionVisible(TargetItem))
		return false;
	
	TArray<UMounteaSelectableInventoryItemAction*> allowedActions = UMounteaInventoryStatics::GetDisplayableItemActions(TargetItem);

	const bool bFound = Algo::FindByPredicate(allowedActions, [this](const UMounteaSelectableInventoryItemAction* action)
	{
		return action && action->GetClass() == GetClass();
	}) != nullptr;

	return bFound;
}

void UMounteaSelectableInventoryItemAction::ExecuteQueuedAction_Implementation(UObject* OptionalPayload)
{
	OnSelectableActionQueueExecuted.Broadcast(this, OptionalPayload);
}

bool UMounteaSelectableInventoryItemAction::IsActionVisible_Implementation(const FMounteaInventoryItem& TargetItem) const
{
	return true;
}