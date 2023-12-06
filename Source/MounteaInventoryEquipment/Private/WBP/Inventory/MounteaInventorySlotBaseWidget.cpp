// All rights reserved Dominik Pavlicek 2023.


#include "WBP/Inventory/MounteaInventorySlotBaseWidget.h"

#include "Interfaces/UI/MounteaDragDropWBPInterface.h"

bool UMounteaInventorySlotBaseWidget::IsSlotEmpty_Implementation() const
{
	// TODO
	return true;
}

FEventReply UMounteaInventorySlotBaseWidget::ResolveDrop_Implementation(UUserWidget* DroppedWidget, UObject* Payload)
{
	FEventReply Result(false);

	if (DroppedWidget == nullptr) return Result;

	TScriptInterface<IMounteaDragDropWBPInterface> DroppedWidgetInterface = DroppedWidget;
	if (DroppedWidgetInterface.GetObject() == nullptr) return Result;

	TScriptInterface<IMounteaInventoryItemWBPInterface> PassedItemWBP = DroppedWidgetInterface->Execute_GetItemWBP(DroppedWidgetInterface.GetObject());
	if (PassedItemWBP.GetObject() == nullptr) return Result;

	
	
	return Result;
}
