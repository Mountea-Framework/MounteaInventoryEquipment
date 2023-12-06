// All rights reserved Dominik Pavlicek 2023.


#include "WBP/Inventory/MounteaInventorySlotBaseWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Interfaces/UI/MounteaDragDropWBPInterface.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

bool UMounteaInventorySlotBaseWidget::IsSlotEmpty_Implementation() const
{
	return ParentSlotGuid.IsValid() == false && AttachedItemWidget == nullptr;
}

FEventReply UMounteaInventorySlotBaseWidget::ResolveDrop_Implementation(UUserWidget* DroppedWidget, UObject* Payload)
{
	FEventReply Result(false);

	if (DroppedWidget == nullptr) return Result;

	TScriptInterface<IMounteaDragDropWBPInterface> DroppedWidgetInterface = DroppedWidget;
	if (DroppedWidgetInterface.GetObject() == nullptr) return Result;

	TScriptInterface<IMounteaInventoryItemWBPInterface> PassedItemWBP = DroppedWidgetInterface->Execute_GetItemWBP(DroppedWidgetInterface.GetObject());
	if (PassedItemWBP.GetObject() == nullptr) return Result;

	// TODO: actually finish the logic :)
	
	return Result;
}

FString UMounteaInventorySlotBaseWidget::GetSlotID() const
{ return SlotID; }
