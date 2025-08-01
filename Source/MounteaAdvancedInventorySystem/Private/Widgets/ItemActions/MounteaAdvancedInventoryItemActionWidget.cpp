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


#include "Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidget.h"

#include "Decorations/MounteaInventoryItemAction.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventoryUIStatics.h"

void UMounteaAdvancedInventoryItemActionWidget::InitializeItemAction_Implementation(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI,
	const TSoftClassPtr<UObject>& ItemActionClass, UWidget* ParentWidget)
{
	ParentUIComponent = ParentUI;
	ActionClass = ItemActionClass;
	ParentItemWidget = ParentWidget;
}

bool UMounteaAdvancedInventoryItemActionWidget::IsActionEnabled_Implementation() const
{
	if (!Execute_IsActionValid(this))
		return false;
	
	return true;
}

bool UMounteaAdvancedInventoryItemActionWidget::IsActionValid_Implementation() const
{
	if (!IsValid(ParentItemWidget))
		return false;

	if (!ParentItemWidget->Implements<UMounteaAdvancedInventoryItemWidgetInterface>())
		return false;
	
	if (!IsValid(ParentUIComponent.GetObject()))
		return false;

	if (!IsValid(ParentUIComponent->Execute_GetParentInventory(ParentUIComponent.GetObject()).GetObject()))
		return false;
	
	return true;
}

void UMounteaAdvancedInventoryItemActionWidget::ExecuteItemAction_Implementation()
{
	if (!IsValid(ParentUIComponent.GetObject()))
	{
		LOG_ERROR(TEXT("[ExecuteItemAction] Invalid or empty `Parent UI` for widget '%s'"), *GetName());
		return;
	}
	
	if (!ActionClass.Get())
	{
		LOG_ERROR(TEXT("[ExecuteItemAction] Invalid or empty `ActionClass` for widget '%s'"), *GetName());
		return;
	}

	UObject* actionInstance = NewObject<UObject>(this, ActionClass.LoadSynchronous());
	if (!actionInstance)
	{
		LOG_ERROR(TEXT("[ExecuteItemAction] Failed to Initialize Item Action for widget '%s'"), *GetName());
		return;
	}
	
	FGuid inventoryItemId = IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryItemId(ParentItemWidget);
	const auto inventoryItem = UMounteaInventoryUIStatics::FindItem(ParentUIComponent, FInventoryItemSearchParams(inventoryItemId));

	IMounteaAdvancedInventoryItemActionInterface::Execute_ExecuteInventoryAction(actionInstance, inventoryItem);
}

FMounteaItemActionData UMounteaAdvancedInventoryItemActionWidget::GetItemActionData_Implementation() const
{
	if (ActionClass.IsNull())
	{
		LOG_ERROR(TEXT("[GetItemActionData] Invalid or empty `ActionClass` for widget '%s'"), *GetName());
		return FMounteaItemActionData();
	}
	const auto actionClass = ActionClass.LoadSynchronous();
	if (!IsValid(actionClass))
	{
		LOG_ERROR(TEXT("[GetItemActionData] Invalid or empty `ActionClass` for widget '%s'"), *GetName());
		return FMounteaItemActionData();
	}

	const auto actionDefault =  GetDefault<UObject>(actionClass);
	return IMounteaAdvancedInventoryItemActionInterface::Execute_GetActionData(actionDefault);
}
