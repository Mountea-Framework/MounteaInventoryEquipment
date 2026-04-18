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

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECraftingStationState : uint8
{
	EASS_Empty			UMETA(DisplayName = "Empty",	Tooltip = "Empty and can be used"),
	EASS_Occupied		UMETA(DisplayName = "Occupied", Tooltip = "Occupied by Crafting Participant"),
	EASS_Inactive		UMETA(DisplayName = "Inactive", Tooltip = "Inactive and can't be used"),

	Default UMETA(Hidden)
};