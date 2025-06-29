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
enum class EAttachmentSlotState : uint8
{
	EASS_Empty			UMETA(DisplayName = "Empty", Tooltip = "Empty Item"),
	EASS_Occupied	UMETA(DisplayName = "Occupied", Tooltip = "Occupied by an Item"),
	EASS_Locked		UMETA(DisplayName = "Locked", Tooltip = "Locked by an Item"),

	Default UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAttachmentSlotType : uint8
{
	EAST_Socket			UMETA(DisplayName = "Socket", Tooltip = "Socket"),
	EAST_Component	UMETA(DisplayName = "Component", Tooltip = "Component"),

	Default UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAttachmentState : uint8
{
	EAS_Attached		UMETA(DisplayName = "Attached", Tooltip = "Attachment is Attached. Slot should be valid."),
	EAS_Detached	UMETA(DisplayName = "Detached", Tooltip = "Attachment is Detached. Slot should be invalid."),

	Default UMETA(Hidden)
};
