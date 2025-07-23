// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidget.h"

#include "Decorations/MounteaInventoryItemAction.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

void UMounteaAdvancedInventoryItemActionWidget::InitializeItemAction_Implementation(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI,
	const TSoftClassPtr<UMounteaInventoryItemAction>& ItemActionClass, const FGuid& ItemId)
{
}

bool UMounteaAdvancedInventoryItemActionWidget::IsActionEnabled_Implementation() const
{
	return false;
}

bool UMounteaAdvancedInventoryItemActionWidget::IsActionValid_Implementation() const
{
	return false;
}

void UMounteaAdvancedInventoryItemActionWidget::ExecuteItemAction_Implementation()
{
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

	// TODO: Find item from Inventory with GUID
	FInventoryItem targetItem = FInventoryItem();
	actionInstance->ExecuteInventoryAction(targetItem);
}
