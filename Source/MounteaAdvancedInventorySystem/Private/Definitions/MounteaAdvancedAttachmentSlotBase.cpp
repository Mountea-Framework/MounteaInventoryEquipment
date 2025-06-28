// Fill out your copyright notice in the Description page of Project Settings.


#include "Definitions/MounteaAdvancedAttachmentSlotBase.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

UMounteaAdvancedAttachmentSlotBase::UMounteaAdvancedAttachmentSlotBase() : 
	SlotName(NAME_None),
	State(EAttachmentSlotState::EASS_Empty),
	SlotType(EAttachmentSlotType::EAST_Socket)
{
}

TArray<FName> UMounteaAdvancedAttachmentSlotBase::GetAvailableSlotNames() const
{
	const auto advanedSettings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!advanedSettings)
		return {};

	const auto equipmentConfig = advanedSettings->EquipmentSettingsConfig.LoadSynchronous();
	if (!IsValid(equipmentConfig))
		return {};

	const auto& allowedSlots = equipmentConfig->AllowedEquipmentSlots;
	TArray<FName> slotNames;
	slotNames.Reserve(allowedSlots.Num());

	Algo::Transform(allowedSlots, slotNames, [](const TPair<FName, FMounteaEquipmentSlotHeaderData>& Pair)
	{
		return Pair.Key;
	});

	return slotNames;
}

#if WITH_EDITOR
void UMounteaAdvancedAttachmentSlotBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	UMounteaAdvancedEquipmentSettingsConfig* equipmentConfig = nullptr;

	if (const auto advanedSettings = GetDefault<UMounteaAdvancedInventorySettings>())
		equipmentConfig = advanedSettings->EquipmentSettingsConfig.LoadSynchronous();
	
	
	const FName propertName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (propertName == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedAttachmentSlotBase, SlotName))
	{
		if (IsValid(equipmentConfig))
		{
			if (const FMounteaEquipmentSlotHeaderData* headerData = equipmentConfig->AllowedEquipmentSlots.Find(SlotName))
			{
				SlotTags.Reset();
				SlotTags.AppendTags(headerData->TagContainer);

				DisplayName = headerData->DisplayName;
			}
		}
	}
}
#endif