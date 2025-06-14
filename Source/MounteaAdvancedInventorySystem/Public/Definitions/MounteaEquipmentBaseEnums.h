// All rights reserved Dominik Morse 2024

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
