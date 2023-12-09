// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/Inventory/MounteaInventorySlotBaseWidget.h"

#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"
#include "Helpers/MounteaItemHelpers.h"
#include "Interfaces/MounteaInventoryInterface.h"

#include "Interfaces/UI/MounteaDragDropWBPInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"
#include "Interfaces/UI/MounteaInventoryWBPInterface.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "MounteaInventorySlotBaseWidget"

void UMounteaInventorySlotBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMounteaInventorySlotBaseWidget::SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory)
{
	if (NewOwningInventory == OwningInventoryWidget) return;

	OwningInventoryWidget = NewOwningInventory;
}

TScriptInterface<IMounteaInventoryWBPInterface> UMounteaInventorySlotBaseWidget::GetOwningInventory_Implementation() const
{
	return OwningInventoryWidget;
}

bool UMounteaInventorySlotBaseWidget::IsSlotEmpty_Implementation() const
{
	return ParentSlotGuid.IsValid() == false && AttachedItemWidget == nullptr;
}

FInventoryUpdateResult UMounteaInventorySlotBaseWidget::AttachItemToSlot_Implementation(UUserWidget* ItemToAttach)
{
	FInventoryUpdateResult Result;

	if (Execute_CanAttach(this, ItemToAttach, Result) == false)
	{
		return Result;
	}
	
	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = ItemToAttach;
	const FGuid NewGuid = NewItem->Execute_GetItem(ItemToAttach).SlotGuid;
	
	ParentSlotGuid = NewGuid;
	AttachedItemWidget = ItemToAttach;
	
	Result.OptionalPayload = this;
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_AttachItem_OK", "Attachement successful.");

	Execute_OnInventorySlotUpdated(this, TEXT("OnAttached"), AttachedItemWidget);
	
	return Result;
}

FInventoryUpdateResult UMounteaInventorySlotBaseWidget::DetachItemFromSlot_Implementation(UUserWidget* ItemToDetach)
{
	FInventoryUpdateResult Result;

	if (Execute_CanDetach(this, ItemToDetach, Result) == false)
	{
		return Result;
	}
		
	Execute_OnInventorySlotUpdated(this, TEXT("OnDetached"), ItemToDetach);
	
	ParentSlotGuid = FGuid();
	AttachedItemWidget = nullptr;
	
	Result.OptionalPayload = this;
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_DetachItem_OK", "Detachement successful.");
	
	return Result;
}

bool UMounteaInventorySlotBaseWidget::CanAttach_Implementation(UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const
{
	if (OwningInventoryWidget.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_OwningInventoryWidget_Invalid", "Owning Equipment UI is not valid!");

		return false;
	}

	const TScriptInterface<IMounteaInventoryInterface> ParentInventory = OwningInventoryWidget->Execute_GetOwningInventory(OwningInventoryWidget.GetObject());
	
	if (ParentInventory.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_OwningInventory_Invalid", "Owning Equipment is not valid!");

		return false;
	}
	
	if (NewChildWidget == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_AttachItem_Invalid", "Cannot attach invalid item!");

		return false;
	}

	if (NewChildWidget == AttachedItemWidget)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_AttachItem_AlreadyAttached", "Cannot attach already attached item!");

		return false;
	}

	if (NewChildWidget->Implements<UMounteaInventoryItemWBPInterface>() == false)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_AttachItem_InvalidInterface", "Cannot attach invalid item!");

		return false;
	}

	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = NewChildWidget;

	if (NewItem.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_AttachItem_SomethingWentWrong", "Unknown issue!");

		return false;
	}

	const FGuid NewGuid = NewItem->Execute_GetItem(NewChildWidget).SlotGuid;

	if (NewGuid == ParentSlotGuid)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_AttachItem_AlreadyAttached", "Cannot attach already attached item!");

		return false;
	}

	return true;
}

bool UMounteaInventorySlotBaseWidget::CanDetach_Implementation(UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const
{
	if (OwningInventoryWidget.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_OwningInventoryWidget_Invalid", "Owning Equipment UI is not valid!");

		return false;
	}

	const TScriptInterface<IMounteaInventoryInterface> ParentInventory = OwningInventoryWidget->Execute_GetOwningInventory(OwningInventoryWidget.GetObject());
	
	if (ParentInventory.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_OwningInventory_Invalid", "Owning Equipment is not valid!");

		return false;
	}
	
	if (OldChildWidget == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_DetachItem_Invalid", "Cannot detach invalid item!");

		return false;
	}

	if (OldChildWidget != AttachedItemWidget)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_DetachItem_AlreadyAttached", "Cannot detach non-attached item!");

		return false;
	}

	if (OldChildWidget->Implements<UMounteaInventoryItemWBPInterface>() == false)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_DetachItem_InvalidInterface", "Cannot detach invalid item!");

		return false;
	}

	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = OldChildWidget;

	if (NewItem.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_DetachItem_SomethingWentWrong", "Unknown issue!");

		return false;
	}

	const FGuid NewGuid = NewItem->Execute_GetItem(OldChildWidget).SlotGuid;

	if (NewGuid != ParentSlotGuid)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaInventorySlotBaseWidget_DetachItem_AlreadyAttached", "Cannot detach non-attached item!");

		return false;
	}

	return true;
}

bool UMounteaInventorySlotBaseWidget::CanDrop_Implementation(UUserWidget* PayloadWidget, UObject* Payload) const
{
	if (PayloadWidget == nullptr) return false;

	TScriptInterface<IMounteaDragDropWBPInterface> PayloadWidgetInterface = PayloadWidget;
	if (PayloadWidgetInterface.GetObject() == nullptr) return false;

	TScriptInterface<IMounteaInventoryItemWBPInterface> PassedItemWBP = PayloadWidgetInterface->Execute_GetItemWBP(PayloadWidgetInterface.GetObject());
	if (PassedItemWBP.GetObject() == nullptr) return false;

	if (PassedItemWBP->GetItemUI() == nullptr) return false;

	TScriptInterface<IMounteaInventoryInterface> Inventory= OwningInventoryWidget->Execute_GetOwningInventory(OwningInventoryWidget.GetObject());
	if (Inventory.GetObject() == nullptr) return false;

	FInventoryUpdateResult Result;
	if (Execute_CanAttach(this, PassedItemWBP->GetItemUI(), Result) == false)
	{
		return false;
	}
	
	const UMounteaInventoryEquipmentSettings* Settings = UMounteaInventoryEquipmentBFL::GetSettings();
	if (Settings == nullptr) return true;

	bool bThemeFound = false;
	const UMounteaInventoryThemeConfig* InventoryUITheme = OwningInventoryWidget->Execute_GetTheme(OwningInventoryWidget.GetObject(), bThemeFound);
	if (InventoryUITheme == nullptr || bThemeFound == false) return true;

	if (InventoryUITheme->InventoryTheme == nullptr) return true;

	return InventoryUITheme->InventoryTheme->bDragDropAllowed;
}

FEventReply UMounteaInventorySlotBaseWidget::ResolveDrop_Implementation(UUserWidget* PayloadWidget, UObject* Payload)
{
	if (Execute_CanDrop(this, PayloadWidget, Payload) == false)
	{
		return false;
	}

	// Validity of those values has been checked in `CanDrop` already, here we assume those are safe to work with
	TScriptInterface<IMounteaDragDropWBPInterface> PayloadWidgetInterface = PayloadWidget;
	TScriptInterface<IMounteaInventoryItemWBPInterface> PassedItemWBP = PayloadWidgetInterface->Execute_GetItemWBP(PayloadWidgetInterface.GetObject());

	Execute_AttachItemToSlot(this, PassedItemWBP->GetItemUI());
	
	return true;
}

#undef LOCTEXT_NAMESPACE