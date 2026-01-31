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
 * EInventoryItemActionCallback defines flags that indicate whether an item action requires additional
 * input or confirmation from the player during execution. These flags help determine if extra UI or
 * game logic is necessary for processing specific item interactions.
 *
 * Flags can be combined using bitwise operations and are used to drive UI prompts and gameplay decisions.
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class EInventoryItemActionCallback : uint8
{
	EIIAC_None                 = 0	UMETA(DisplayName = "None", Tooltip = "No additional input or confirmation required."),
	EIIAC_QuantityNeeded       = 1 << 0	UMETA(DisplayName = "Quantity Needed", Tooltip = "Player must specify the quantity to use or move."),
	EIIAC_ConfirmationNeeded   = 1 << 1	UMETA(DisplayName = "Confirmation Needed", Tooltip = "Requires user confirmation before proceeding.")
};
ENUM_CLASS_FLAGS(EInventoryItemActionCallback)

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
