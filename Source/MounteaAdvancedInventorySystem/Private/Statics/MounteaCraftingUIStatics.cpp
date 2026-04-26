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


#include "Statics/MounteaCraftingUIStatics.h"

#include "Settings/MounteaAdvancedCraftingUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

UMounteaAdvancedCraftingUIConfig* UMounteaCraftingUIStatics::GetCraftingUISettingsConfig()
{
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!settings) return nullptr;
	return settings->AdvancedCraftingUISettingsConfig.LoadSynchronous();
}
