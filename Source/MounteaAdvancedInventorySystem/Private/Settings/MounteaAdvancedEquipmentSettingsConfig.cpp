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

#include "Algo/ForEach.h"

#if WITH_EDITOR
void UMounteaAdvancedEquipmentSettingsConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() !=
		GET_MEMBER_NAME_CHECKED(UMounteaAdvancedEquipmentSettingsConfig, AllowedEquipmentSlots))
	{
		return;
	}

	Algo::ForEach(AllowedEquipmentSlots, [](TPair<FName, FMounteaEquipmentSlotHeaderData>& Pair)
	{
		Pair.Value.RegenerateSlotId();
	});
}
#endif
