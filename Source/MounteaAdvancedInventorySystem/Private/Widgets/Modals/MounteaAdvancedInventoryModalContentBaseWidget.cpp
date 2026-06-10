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

#include "Widgets/Modals/MounteaAdvancedInventoryModalContentBaseWidget.h"

#include "Helpers/MounteaModalsPayload.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

void UMounteaAdvancedInventoryModalContentBaseWidget::OnModalExpired()
{
	OnModalContentCancelled.Broadcast();
}

void UMounteaAdvancedInventoryModalContentBaseWidget::ConstructModalContent_Implementation(UMounteaModalsPayload* Payload)
{
	if (!IsValid(Payload))
	{
		LOG_ERROR(TEXT("[Construct Modal Content] Invalid Modal Payload Provided."))
		return;
	}
	
	if (Payload->ModalConfig.bAutoClose && Payload->ModalConfig.ModalDuration > 0)
	{
		ModalContentExpiryTimerHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(ModalContentExpiryTimerHandle, this, &UMounteaAdvancedInventoryModalContentBaseWidget::OnModalExpired, Payload->ModalConfig.ModalDuration);
	}
	
	ModalType = Payload->ModalType;
}
