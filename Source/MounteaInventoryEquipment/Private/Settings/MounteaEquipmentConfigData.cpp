// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Settings/MounteaEquipmentConfigData.h"

#if WITH_EDITOR

void UMounteaEquipmentConfigData::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	// TODO: If EquipmentSlotIDs are about to change, we must cache out all places where current value is used and update it to the new value
}

void UMounteaEquipmentConfigData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// TODO: If EquipmentSlotIDs are about to change, we must cache out all places where current value is used and update it to the new value
}

#endif
