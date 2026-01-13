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

#include "Blueprint/UserWidget.h"
#include "Definitions/MounteaInventoryBaseUIEnums.h"
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

bool UMounteaInventoryItemAction::IsActionVisible_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	return ItemActionData.bIsVisibleByDefault;
}

bool UMounteaInventoryItemAction::IsAllowed_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	if (!IsActionVisible(TargetItem))
		return false;

	if (!IsValid(TargetItem.Template))
		return false;

	auto allowedActions = UMounteaInventoryStatics::GetItemActions(TargetItem);

	const bool bFound = Algo::FindByPredicate(allowedActions, [this](const UMounteaInventoryItemAction* action)
	{
		return action && action->GetClass() == GetClass();
	}) != nullptr;

	return bFound;
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
	
	return ProcessAction(this, TargetItem);
}

bool UMounteaInventoryItemAction::ProcessAction_Implementation(UObject* ActionInitiator,
	const FInventoryItem& TargetItem)
{
	LOG_WARNING(TEXT("[ProcessAction] Called on base simple action class for %s!"), *GetName())
	return false;
}

EInventoryItemActionCallback UMounteaInventoryItemAction::GetInventoryItemActionCallback_Implementation() const
{
	return static_cast<EInventoryItemActionCallback>(ItemActionData.InventoryItemActionCallback);
}

void UMounteaInventoryItemAction::AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd)
{
	ItemActionData.InventoryItemActionCallback |= static_cast<uint8>(FlagToAdd);
}

void UMounteaInventoryItemAction::RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove)
{
	ItemActionData.InventoryItemActionCallback &= ~static_cast<uint8>(FlagToRemove);
}

void UMounteaInventoryItemAction::ClearAllActionFlags_Implementation()
{
	ItemActionData.InventoryItemActionCallback = 0;
}

