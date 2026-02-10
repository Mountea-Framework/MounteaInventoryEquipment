// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"

#include "Algo/AnyOf.h"
#include "Algo/ForEach.h"

UMounteaAdvancedEquipmentSettingsConfig::UMounteaAdvancedEquipmentSettingsConfig()
{
	AllowedAttachmentTargets.Add(TSoftClassPtr<USceneComponent>(USceneComponent::StaticClass()));
	AllowedAttachmentTargets.Add(TSoftClassPtr<USceneComponent>(UStaticMeshComponent::StaticClass()));
	AllowedAttachmentTargets.Add(TSoftClassPtr<USceneComponent>(USkeletalMeshComponent::StaticClass()));
}

TArray<FName> UMounteaAdvancedEquipmentSettingsConfig::GetFallbackSlotOptions() const
{
	TArray<FName> returnValue;
	returnValue.Reserve(AllowedEquipmentSlots.Num() + 1);
	returnValue.Add(NAME_None);
	for (const auto& allowedSlot : AllowedEquipmentSlots)
	{
		returnValue.Add(allowedSlot.Key);
	}
	return returnValue;
}

#if WITH_EDITOR

void UMounteaAdvancedEquipmentSettingsConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() !=
		GET_MEMBER_NAME_CHECKED(UMounteaAdvancedEquipmentSettingsConfig, AllowedEquipmentSlots))
	{
		return;
	}

	Algo::ForEach(AllowedEquipmentSlots, [this](TPair<FName, FMounteaEquipmentSlotHeaderData>& equipmentSlot)
	{
		const FName fallbackSlot = equipmentSlot.Value.FallbackSlot;
		const bool bIsSelfFallback = (fallbackSlot == equipmentSlot.Key);
		const bool bFallbackExists = fallbackSlot.IsNone() ? true : Algo::AnyOf(
			AllowedEquipmentSlots,
			[fallbackSlot](const TPair<FName, FMounteaEquipmentSlotHeaderData>& otherSlot)
			{
				return otherSlot.Key == fallbackSlot;
			});

		if (bIsSelfFallback || !bFallbackExists)
			equipmentSlot.Value.FallbackSlot = NAME_None;

		equipmentSlot.Value.RegenerateSlotId();
	});
}
#endif
