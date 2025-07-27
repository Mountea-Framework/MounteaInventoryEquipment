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


#include "Decorations/MounteaInventorySimpleItemAction.h"

#include "Logs/MounteaAdvancedInventoryLog.h"

bool UMounteaInventorySimpleItemAction::InitializeItemAction_Implementation(const FInventoryItem& NewTargetItem,
																			const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewOwningInventory)
{
	if (!NewTargetItem.IsItemValid() || NewTargetItem.OwningInventory != NewOwningInventory || !IsValid(NewOwningInventory.GetObject()))
		return false;
	
	CurrentTargetItem = NewTargetItem;

	return true;
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventorySimpleItemAction::
GetOwningInventory_Implementation() const
{
	if (CurrentTargetItem.IsItemValid())
		return CurrentTargetItem.GetOwningInventory();

	return nullptr;
}

bool UMounteaInventorySimpleItemAction::IsActionVisible_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	return ItemActionData.bIsVisibleByDefault;
}

bool UMounteaInventorySimpleItemAction::IsAllowed_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	if (!IsActionVisible(TargetItem))
		return false;

	return true;
}

FText UMounteaInventorySimpleItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return FText::FromString(TEXT("Invalid target item"));

	return FText::FromString(TEXT("Action is not currently available"));
}

bool UMounteaInventorySimpleItemAction::ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem)
{
	if (!IsAllowed(TargetItem))
	{
		LOG_WARNING(TEXT("[%s] Action not allowed: %s"), *GetName(), *GetDisallowedReason(TargetItem).ToString())
		return false;
	}
	
	return Execute_ProcessAction(this, this, TargetItem);
}

bool UMounteaInventorySimpleItemAction::ProcessAction_Implementation(UObject* ActionInitiator,
	const FInventoryItem& TargetItem)
{
	LOG_WARNING(TEXT("[ProcessAction] Called on base simple action class for %s!"), *GetName())
	return false;
}
