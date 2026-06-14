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

#include "MounteaInventoryBaseUIEnums.generated.h"

/**
 * Basically 1:1 copy of `EInputActionValueType` for now.
 */
UENUM(BlueprintType)
enum class EMounteaWidgetInputMethod : uint8
{
	// Value types in increasing size order (used for type promotion)
	// Name these Digital/Analog?

	Boolean				UMETA(DisplayName = "Digital (bool)"),
	Axis1D				UMETA(DisplayName = "Axis1D (float)"),
	Axis2D				UMETA(DisplayName = "Axis2D (Vector2D)"),
	Axis3D				UMETA(DisplayName = "Axis3D (Vector)"),
};

/**
 * Describes the physical source/type of the widget input.
 */
UENUM(BlueprintType)
enum class EMounteaWidgetInputKey : uint8
{
	Key 				UMETA(DisplayName = "Key"),
	Analog 				UMETA(DisplayName = "Analog"),
	Wheel 				UMETA(DisplayName = "Wheel"),
	Touch 				UMETA(DisplayName = "Touch"),
};