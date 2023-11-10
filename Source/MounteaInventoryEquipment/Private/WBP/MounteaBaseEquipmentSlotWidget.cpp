// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/MounteaBaseEquipmentSlotWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"
#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

TArray<FString> UMounteaBaseEquipmentSlotWidget::GetSlotIDOptions()
{
	TArray<FString> Results;

	if (const UMounteaInventoryEquipmentSettings* const Settings = UMounteaInventoryEquipmentBPF::GetSettings())
	{
		if (!Settings->EquipmentConfigData.IsNull())
		{
			Settings->EquipmentConfigData.LoadSynchronous()->EquipmentSlotIDs.GetKeys(Results);
		}
	}

	return Results;
}
