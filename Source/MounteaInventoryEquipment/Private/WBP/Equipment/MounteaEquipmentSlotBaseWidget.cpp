// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/Equipment/MounteaEquipmentSlotBaseWidget.h"

#include "Components/PanelWidget.h"
#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Helpers/MounteaItemHelpers.h"
#include "Interfaces/UI/MounteaEquipmentWBPInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"
#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE ""

void UMounteaEquipmentSlotBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TScriptInterface<IMounteaEquipmentWBPInterface> UMounteaEquipmentSlotBaseWidget::GetOwningEquipment_Implementation() const
{
	return OwningEquipmentWidget;
}

void UMounteaEquipmentSlotBaseWidget::SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentWBPInterface>& NewOwningEquipment)
{
	if (OwningEquipmentWidget == NewOwningEquipment) return;

	OwningEquipmentWidget = NewOwningEquipment;
}

bool UMounteaEquipmentSlotBaseWidget::IsSlotEmpty_Implementation() const
{
	return  ParentSlotGuid.IsValid() == false && AttachedItemWidget == nullptr;
}

FInventoryUpdateResult UMounteaEquipmentSlotBaseWidget::AttachItemToSlot_Implementation(UUserWidget* ItemToAttach)
{
	FInventoryUpdateResult Result;
	
	if (ItemToAttach == nullptr)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_Invalid", "Cannot attach invalid item!");

		return Result;
	}

	if (ItemToAttach == AttachedItemWidget)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_AlreadyAttached", "Cannot attach already attached item!");

		return Result;
	}

	if (ItemToAttach->Implements<UMounteaInventoryItemWBPInterface>() == false)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_InvalidInterface", "Cannot attach invalid item!");

		return Result;
	}

	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = ItemToAttach;

	if (NewItem.GetObject() == nullptr)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_SomethingWentWrong", "Unknown issue!");

		return Result;
	}

	const FGuid NewGuid = NewItem->Execute_GetItem(ItemToAttach).SlotGuid;

	if (NewGuid == ParentSlotGuid)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_AlreadyAttached", "Cannot attach already attached item!");

		return Result;
	}

	ParentSlotGuid = NewGuid;
	AttachedItemWidget = ItemToAttach;
	
	Result.OptionalPayload = this;
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_OK", "Equipment successful.");
	
	return Result;
}

FInventoryUpdateResult UMounteaEquipmentSlotBaseWidget::DetachItemToSlot_Implementation(UUserWidget* ItemToDetach)
{
	FInventoryUpdateResult Result;
	
	if (ItemToDetach == nullptr)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_Invalid", "Cannot detach invalid item!");

		return Result;
	}

	if (ItemToDetach != AttachedItemWidget)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_AlreadyAttached", "Cannot detach non-attached item!");

		return Result;
	}

	if (ItemToDetach->Implements<UMounteaInventoryItemWBPInterface>() == false)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_InvalidInterface", "Cannot detach invalid item!");

		return Result;
	}

	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = ItemToDetach;

	if (NewItem.GetObject() == nullptr)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_SomethingWentWrong", "Unknown issue!");

		return Result;
	}

	const FGuid NewGuid = NewItem->Execute_GetItem(ItemToDetach).SlotGuid;

	if (NewGuid != ParentSlotGuid)
	{
		Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_AlreadyAttached", "Cannot detach non-attached item!");

		return Result;
	}

	ParentSlotGuid = FGuid();
	AttachedItemWidget = nullptr;
	
	Result.OptionalPayload = this;
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_OK", "Detachement successful.");
	
	return Result;
}

void UMounteaEquipmentSlotBaseWidget::UpdateSlotID(const FGameplayTag& AffectedSlot)
{
	if (const UMounteaInventoryEquipmentSettings* const Settings = UMounteaInventoryEquipmentBPF::GetSettings())
	{
		if (!Settings->EquipmentConfigData.IsNull())
		{
			const auto IDs = Settings->EquipmentConfigData.LoadSynchronous()->EquipmentSlotIDs;
			if (IDs.Contains(AffectedSlot))
			{
				SlotID = IDs.Find(AffectedSlot)->GetSlotID();

				return;
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("[EquipmentSlot - UpdateSlotID] Slot %s is not supported!"), *AffectedSlot.ToString())
}

TArray<FString> UMounteaEquipmentSlotBaseWidget::GetSlotIDOptions()
{
	TArray<FString> Results;

	if (const UMounteaInventoryEquipmentSettings* const Settings = UMounteaInventoryEquipmentBPF::GetSettings())
	{
		if (!Settings->EquipmentConfigData.IsNull())
		{
			const auto IDs = Settings->EquipmentConfigData.LoadSynchronous()->EquipmentSlotIDs;
			for (const auto& Itr : IDs)
			{
				if (Itr.SlotTag.IsValid() == false) continue;
				
				Results.Add(Itr.GetSlotID());
			}
		}
	}

	return Results;
}

#undef LOCTEXT_NAMESPACE