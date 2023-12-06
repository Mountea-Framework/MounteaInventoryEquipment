// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/Equipment/MounteaEquipmentSlotBaseWidget.h"

#include "Definitions/MounteaInventoryInstancedItem.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Helpers/MounteaItemHelpers.h"
#include "Interfaces/MounteaEquipmentInterface.h"

#include "Interfaces/UI/MounteaEquipmentWBPInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"

#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "MounteaEquipmentSlotBaseWidget"

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

	if (Execute_CanAttach(this, ItemToAttach, Result) == false)
	{
		return Result;
	}

	if (Execute_IsSlotEmpty(this) == false)
	{
		if (Execute_CanDetach(this, AttachedItemWidget, Result) == false)
		{
			Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
			Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_Invalid", "Cannot attach item, already equipped one cannot be detached.");

			return Result;
		}
		
		Execute_DetachItemToSlot(this, AttachedItemWidget);	
	}
	
	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = ItemToAttach;
	const FGuid NewGuid = NewItem->Execute_GetItem(ItemToAttach).SlotGuid;

	// TODO: Consider moving this to replicated wrapper as the Item Flag can be important for replication
	if (NewItem.GetObject())
	{
		if (const UMounteaInventoryEquipmentSettings* const Settings = UMounteaInventoryEquipmentBPF::GetSettings())
		{
			if (const UMounteaEquipmentConfigData* EquipmentSettings = Settings->EquipmentConfigData.Get())
			{
				if (const auto Item = NewItem->Execute_GetItem(NewItem.GetObject()).Item )
				{
					Item->AddItemFlag(EquipmentSettings->EquippedFlag);
				}
			}
		}
	}
	
	ParentSlotGuid = NewGuid;
	AttachedItemWidget = ItemToAttach;
	
	Result.OptionalPayload = this;
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_OK", "Equipment successful.");

	Execute_OnEquipmentSlotUpdated(this, TEXT("OnAttached"), AttachedItemWidget);
	
	return Result;
}

FInventoryUpdateResult UMounteaEquipmentSlotBaseWidget::DetachItemFromSlot_Implementation(UUserWidget* ItemToDetach)
{
	FInventoryUpdateResult Result;

	if (Execute_CanDetach(this, ItemToDetach, Result) == false)
	{
		return Result;
	}
		
	Execute_OnEquipmentSlotUpdated(this, TEXT("OnDetached"), ItemToDetach);

	const TScriptInterface<IMounteaInventoryItemWBPInterface> OldItem = ItemToDetach;

	// Consider removing this as flags are replicated through the Equipment itself, but we can assume that equipment will be updated
	// so we can safely remove the flag locally.
	if (OldItem.GetObject())
	{
		if (const UMounteaInventoryEquipmentSettings* const Settings = UMounteaInventoryEquipmentBPF::GetSettings())
		{
			if (const UMounteaEquipmentConfigData* EquipmentSettings = Settings->EquipmentConfigData.Get())
			{
				if (const auto Item = OldItem->Execute_GetItem(OldItem.GetObject()).Item )
				{
					Item->RemoveItemFlag(EquipmentSettings->EquippedFlag);
				}
			}
		}
	}

	ParentSlotGuid = FGuid();
	AttachedItemWidget = nullptr;
	
	Result.OptionalPayload = this;
	Result.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_OK;
	Result.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_OK", "Detachement successful.");
	
	return Result;
}

bool UMounteaEquipmentSlotBaseWidget::CanAttach_Implementation(UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const
{
	if (OwningEquipmentWidget.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_OwningEquipmentWidget_Invalid", "Owning Equipment UI is not valid!");

		return false;
	}

	const TScriptInterface<IMounteaEquipmentInterface> ParentEquipment = OwningEquipmentWidget->Execute_GetOwningEquipment(OwningEquipmentWidget.GetObject());
	
	if (ParentEquipment.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_OwningEquipment_Invalid", "Owning Equipment is not valid!");

		return false;
	}
	
	if (NewChildWidget == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_Invalid", "Cannot attach invalid item!");

		return false;
	}

	if (NewChildWidget == AttachedItemWidget)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_AlreadyAttached", "Cannot attach already attached item!");

		return false;
	}

	if (NewChildWidget->Implements<UMounteaInventoryItemWBPInterface>() == false)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_InvalidInterface", "Cannot attach invalid item!");

		return false;
	}

	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = NewChildWidget;

	if (NewItem.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_SomethingWentWrong", "Unknown issue!");

		return false;
	}

	const FGuid NewGuid = NewItem->Execute_GetItem(NewChildWidget).SlotGuid;

	if (NewGuid == ParentSlotGuid)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_AttachItem_AlreadyAttached", "Cannot attach already attached item!");

		return false;
	}

	return true;
}

bool UMounteaEquipmentSlotBaseWidget::CanDetach_Implementation(UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const
{
	if (OwningEquipmentWidget.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_OwningEquipmentWidget_Invalid", "Owning Equipment UI is not valid!");

		return false;
	}

	const TScriptInterface<IMounteaEquipmentInterface> ParentEquipment = OwningEquipmentWidget->Execute_GetOwningEquipment(OwningEquipmentWidget.GetObject());
	
	if (ParentEquipment.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_OwningEquipment_Invalid", "Owning Equipment is not valid!");

		return false;
	}
	
	if (OldChildWidget == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_Invalid", "Cannot detach invalid item!");

		return false;
	}

	if (OldChildWidget != AttachedItemWidget)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_AlreadyAttached", "Cannot detach non-attached item!");

		return false;
	}

	if (OldChildWidget->Implements<UMounteaInventoryItemWBPInterface>() == false)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_InvalidInterface", "Cannot detach invalid item!");

		return false;
	}

	const TScriptInterface<IMounteaInventoryItemWBPInterface> NewItem = OldChildWidget;

	if (NewItem.GetObject() == nullptr)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_GenericIssue;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_SomethingWentWrong", "Unknown issue!");

		return false;
	}

	const FGuid NewGuid = NewItem->Execute_GetItem(OldChildWidget).SlotGuid;

	if (NewGuid != ParentSlotGuid)
	{
		OutResult.ResultID = MounteaInventoryEquipmentConsts::InventoryUpdatedCodes::Status_BadRequest;
		OutResult.ResultText = LOCTEXT("MounteaEquipmentSlotBaseWidget_DetachItem_AlreadyAttached", "Cannot detach non-attached item!");

		return false;
	}

	return true;
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