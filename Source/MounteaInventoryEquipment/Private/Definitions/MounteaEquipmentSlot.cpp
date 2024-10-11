// All rights reserved Dominik Morse (Pavlicek) 2024


#include "Definitions/MounteaEquipmentSlot.h"

#include "Misc/DataValidation.h"
#include "Settings/MounteaEquipmentConfigData.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "EquipmentSlot"


TArray<FString> UMounteaEquipmentSlot::GetSlotNames()
{
	TArray<FString> ReturnValues;
	if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
	{
		if (const UMounteaEquipmentConfigData* EquipmentData = Settings->EquipmentConfigData.LoadSynchronous())
		{
			EquipmentData->EquipmentSlotIDs.GetKeys(ReturnValues);
		} 
	}

	return ReturnValues;
}

#if WITH_EDITOR
void UMounteaEquipmentSlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	FDataValidationContext validationContext;
	IsDataValid(validationContext);
	
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UMounteaEquipmentSlot, SlotName))
	{
		/*
		if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
		{
			if (const UMounteaEquipmentConfigData* EquipmentData = Settings->EquipmentConfigData.LoadSynchronous())
			{
				if (EquipmentData->EquipmentSlotIDs.Contains(SlotName))
				{
					SlotCompatibleTag = *EquipmentData->EquipmentSlotIDs.Find(SlotName);
				}
			} 
		}
		*/
	}
}

EDataValidationResult UMounteaEquipmentSlot::IsDataValid(FDataValidationContext& Context)
{
	if (DefaultSlotState == EEquipmentSlotState::ESS_Occupied)
	{
		Context.AddError(LOCTEXT("EquipmentSlotValidation_DefaultEquipmentSlotState", "DefaultEquipmentSlotState cannot be Occupied!"));
	}
	if (!SlotGuid.IsValid())
	{
		Context.AddError(LOCTEXT("EquipmentSlotValidation_SlotGuid", "SlotGuid is not valid!"));
	}
	if (SlotName.IsEmpty())
	{
		Context.AddError(LOCTEXT("EquipmentSlotValidation_SlotName", "SlotName is not valid!"));
	}
	if (SlotCompatibleTags.IsEmpty())
	{
		Context.AddError(LOCTEXT("EquipmentSlotValidation_SlotCompatibleTags", "SlotCompatibleTags are not valid!"));
	}
	return UObject::IsDataValid(Context);
}
#endif

#undef LOCTEXT_NAMESPACE