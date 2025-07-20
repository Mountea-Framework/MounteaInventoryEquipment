// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemActions/MounteaAdvancedInventoryItemActionWidget.h"

void UMounteaAdvancedInventoryItemActionWidget::InitializeItemAction_Implementation(
	const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, TSubclassOf<UMounteaInventoryItemAction> ItemAction,
	const FGuid& ItemId)
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
