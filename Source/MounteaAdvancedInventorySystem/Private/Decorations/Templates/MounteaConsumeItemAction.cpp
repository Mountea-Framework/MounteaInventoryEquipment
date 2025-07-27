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

#include "Decorations/Templates/MounteaConsumeItemAction.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

UMounteaConsumeItemAction::UMounteaConsumeItemAction()
{
	ItemActionData.ActionDisplayName = FText::FromString(TEXT("Consume"));
	ItemActionData.ActionDescription = FText::FromString(TEXT("Consume this item"));
	ItemActionData.ActionPriority = 100;
	ItemActionData.bRequiresConfirmation = false;
}

bool UMounteaConsumeItemAction::IsActionVisible_Implementation(const FInventoryItem& TargetItem) const
{
	if (!TargetItem.IsItemValid())
		return false;

	const UMounteaInventoryItemTemplate* ItemTemplate = TargetItem.GetTemplate();
	if (!ItemTemplate)
		return false;

	return (ItemTemplate->ItemFlags & static_cast<uint8>(EInventoryItemFlags::EIIF_Consumable)) != 0;
}

bool UMounteaConsumeItemAction::IsAllowed_Implementation(const FInventoryItem& TargetItem) const
{
	if (!Super::IsAllowed_Implementation(TargetItem))
		return false;

	return TargetItem.GetQuantity() > 0;
}

bool UMounteaConsumeItemAction::ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem)
{
	TScriptInterface<IMounteaAdvancedInventoryInterface> owningInventory = TargetItem.GetOwningInventory();
	if (!owningInventory.GetObject())
	{
		LOG_ERROR(TEXT("[ConsumeItemAction] No owning inventory found"))
		return false;
	}

	const bool success = owningInventory->Execute_DecreaseItemQuantity(owningInventory.GetObject(), TargetItem.GetGuid(), 1);
	
	if (success)
		LOG_INFO(TEXT("[ConsumeItemAction] Successfully consumed item %s"), *TargetItem.GetGuid().ToString())
	else
		LOG_WARNING(TEXT("[ConsumeItemAction] Failed to consume item %s"), *TargetItem.GetGuid().ToString())

	return success;
}

FText UMounteaConsumeItemAction::GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const
{
	if (TargetItem.GetQuantity() <= 0)
		return FText::FromString(TEXT("No items remaining to consume"));

	return Super::GetDisallowedReason_Implementation(TargetItem);
}