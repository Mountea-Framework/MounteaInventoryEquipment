// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/MounteaEquipmentSlotBaseWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

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
				Results.Add(Itr.GetSlotID());
			}
		}
	}

	return Results;
}
