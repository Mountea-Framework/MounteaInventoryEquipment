// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidget.h"

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
}
