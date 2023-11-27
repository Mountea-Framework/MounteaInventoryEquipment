// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/MounteaEquipmentSlotBaseWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

TArray<FText> UMounteaEquipmentSlotBaseWidget::GetSlotIDOptions()
{
	TArray<FText> Results;

	if (const UMounteaInventoryEquipmentSettings* const Settings = UMounteaInventoryEquipmentBPF::GetSettings())
	{
		if (!Settings->EquipmentConfigData.IsNull())
		{
			const auto IDs = Settings->EquipmentConfigData.LoadSynchronous()->EquipmentSlotIDs;
			for (const auto& Itr : IDs)
			{
				Results.Add(Itr.SlotName);
			}
		}
	}

	return Results;
}
