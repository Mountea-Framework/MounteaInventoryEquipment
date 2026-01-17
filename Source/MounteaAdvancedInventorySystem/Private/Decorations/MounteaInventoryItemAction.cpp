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


#include "Decorations/MounteaInventoryItemAction.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventoryStatics.h"

bool UMounteaInventoryItemAction::InitializeItemAction_Implementation(const FInventoryItem& NewTargetItem,
 const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewOwningInventory, UObject* ContextPayload)
{
	if (!NewTargetItem.IsItemValid() || NewTargetItem.OwningInventory != NewOwningInventory || !IsValid(NewOwningInventory.GetObject()))
		return false;
	
	return true;
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaInventoryItemAction::
GetOwningInventory_Implementation() const
{
	return nullptr;
}

bool UMounteaInventoryItemAction::IsAllowed_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	if (!IsValid(TargetItem.Template))
		return false;

	return false;
}

FText UMounteaInventoryItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return FText::FromString(TEXT("Invalid target item"));

	return FText::FromString(TEXT("Action is not currently available"));
}

bool UMounteaInventoryItemAction::ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem)
{
	if (!IsAllowed(TargetItem))
	{
		LOG_WARNING(TEXT("[%s] Action not allowed: %s"), *GetName(), *GetDisallowedReason(TargetItem).ToString())
		return false;
	}
	
	LOG_WARNING(TEXT("[ExecuteInventoryAction] Called on action class for %s!"), *GetName())
	return true;
}

