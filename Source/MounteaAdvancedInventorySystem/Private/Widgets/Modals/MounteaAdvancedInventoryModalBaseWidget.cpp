// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Widgets/Modals/MounteaAdvancedInventoryModalBaseWidget.h"

#include "Components/PanelWidget.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"

void UMounteaAdvancedInventoryModalBaseWidget::AddModalContentToModalWindow_Implementation(UUserWidget* ModalContentWidget, UMounteaJsonObject* Payload)
{
	if (!IsValid(ModalContentWidget))
		return;

	if (IMounteaAdvancedInventoryModalContentWidgetInterface* previousContent = Cast<IMounteaAdvancedInventoryModalContentWidgetInterface>(CurrentModalContentWidget))
	{
		previousContent->GetOnModalContentConfirmedHandle().RemoveDynamic(this, &UMounteaAdvancedInventoryModalBaseWidget::HandleModalContentConfirmed);
		previousContent->GetOnModalContentCancelledHandle().RemoveDynamic(this, &UMounteaAdvancedInventoryModalBaseWidget::HandleModalContentCancelled);
	}

	CurrentModalContentWidget = ModalContentWidget;

	if (IMounteaAdvancedInventoryModalContentWidgetInterface* newContent = Cast<IMounteaAdvancedInventoryModalContentWidgetInterface>(CurrentModalContentWidget))
	{
		newContent->GetOnModalContentConfirmedHandle().AddUniqueDynamic(this, &UMounteaAdvancedInventoryModalBaseWidget::HandleModalContentConfirmed);
		newContent->GetOnModalContentCancelledHandle().AddUniqueDynamic(this, &UMounteaAdvancedInventoryModalBaseWidget::HandleModalContentCancelled);
	}
	
	OnModalContentAddedToModalWindow.Broadcast(ModalContentWidget, Payload);
}

void UMounteaAdvancedInventoryModalBaseWidget::HandleModalContentConfirmed(UObject* Payload)
{
	Execute_ModalContentConfirmed(this, Payload);
	OnModalConfirmed.Broadcast(Payload);
}

void UMounteaAdvancedInventoryModalBaseWidget::HandleModalContentCancelled()
{
	Execute_ModalContentCancelled(this);
	OnModalCancelled.Broadcast();
}
