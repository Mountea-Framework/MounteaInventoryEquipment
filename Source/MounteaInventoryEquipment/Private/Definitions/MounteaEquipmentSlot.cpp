// All rights reserved Dominik Pavlicek 2023.


#include "Definitions/MounteaEquipmentSlot.h"

#include "Settings/MounteaInventoryEquipmentSettings.h"

TArray<FString> UMounteaEquipmentSlot::GetSlotIDs() const
{
	TArray<FString> ReturnValues;
	if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
	{
		Settings->EquipmentSlotIDs.GetKeys(ReturnValues);
	}

	return ReturnValues;
}

#if WITH_EDITOR
void UMounteaEquipmentSlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UMounteaEquipmentSlot, SlotID))
	{
		if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
		{
			SlotCompatibleTag = *Settings->EquipmentSlotIDs.Find(SlotID);
		}
	}
}
#endif
