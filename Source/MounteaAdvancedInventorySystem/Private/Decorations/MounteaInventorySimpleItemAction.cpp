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

#include "Blueprint/UserWidget.h"
#include "Definitions/MounteaInventoryBaseUIEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventoryStatics.h"

bool UMounteaInventorySimpleItemAction::InitializeItemAction_Implementation(const FInventoryItem& NewTargetItem,
 const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewOwningInventory, UObject* ContextPayload)
{
	if (!NewTargetItem.IsItemValid() || NewTargetItem.OwningInventory != NewOwningInventory || !IsValid(NewOwningInventory.GetObject()))
		return false;
	
	CurrentTargetItem = NewTargetItem;

	if (TObjectPtr<UUserWidget> payloadWidget = Cast<UUserWidget>(ContextPayload))
		ParentItemWidget = payloadWidget;
	else
		LOG_WARNING(TEXT("[%s] Invalid context payload for item action!"), *GetName())
	
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

	if (!Execute_IsActionVisible(this, TargetItem))
		return false;

	if (!IsValid(TargetItem.Template))
		return false;

	auto allowedActions = UMounteaInventoryStatics::GetItemActions(CurrentTargetItem);

	const bool bFound = Algo::FindByPredicate(allowedActions, [this](const TSoftClassPtr<UObject>& action)
	{
		return action == GetClass();
	}) != nullptr;

	return bFound;
}

FText UMounteaInventorySimpleItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return FText::FromString(TEXT("Invalid target item"));

	return FText::FromString(TEXT("Action is not currently available"));
}

bool UMounteaInventorySimpleItemAction::ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem)
{
	if (!Execute_IsAllowed(this, TargetItem))
	{
		LOG_WARNING(TEXT("[%s] Action not allowed: %s"), *GetName(), *Execute_GetDisallowedReason(this, TargetItem).ToString())
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

EInventoryItemActionCallback UMounteaInventorySimpleItemAction::GetInventoryItemActionCallback_Implementation() const
{
	return static_cast<EInventoryItemActionCallback>(ItemActionData.InventoryItemActionCallback);
}

void UMounteaInventorySimpleItemAction::AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd)
{
	ItemActionData.InventoryItemActionCallback |= static_cast<uint8>(FlagToAdd);
}

void UMounteaInventorySimpleItemAction::RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove)
{
	ItemActionData.InventoryItemActionCallback &= ~static_cast<uint8>(FlagToRemove);
}

void UMounteaInventorySimpleItemAction::ClearAllActionFlags_Implementation()
{
	ItemActionData.InventoryItemActionCallback = 0;
}

