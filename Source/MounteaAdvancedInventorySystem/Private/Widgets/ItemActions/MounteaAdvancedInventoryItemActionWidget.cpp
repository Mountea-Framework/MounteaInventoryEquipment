// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidget.h"

#include "Decorations/MounteaInventoryItemAction.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaInventoryUIStatics.h"

void UMounteaAdvancedInventoryItemActionWidget::InitializeItemAction_Implementation(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI,
	const TSoftClassPtr<UMounteaInventoryItemAction>& ItemActionClass, UUserWidget* ParentWidget)
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
	
	if (!ActionClass.Get() || ActionClass.LoadSynchronous())
	{
		LOG_ERROR(TEXT("[ExecuteItemAction] Invalid or empty `ActionClass` for widget '%s'"), *GetName());
		return;
	}

	UMounteaInventoryItemAction* actionInstance = NewObject<UMounteaInventoryItemAction>(this, ActionClass.LoadSynchronous());
	if (!actionInstance)
	{
		LOG_ERROR(TEXT("[ExecuteItemAction] Failed to Initialize Item Action for widget '%s'"), *GetName());
		return;
	}

	// TODO: Get the proper guid
	FGuid inventoryItemId = IMounteaAdvancedInventoryItemWidgetInterface::Execute_GetInventoryItemId(ParentItemWidget);
	const auto inventoryItem = UMounteaInventoryUIStatics::FindItem(ParentUIComponent, FInventoryItemSearchParams(inventoryItemId));
	
	actionInstance->ExecuteInventoryAction(inventoryItem);
}
