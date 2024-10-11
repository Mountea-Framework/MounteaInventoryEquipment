// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MounteaInventoryFlagsLibrary.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EInventoryItemFlags : uint8
{
	None        = 0,
	Tradeable   = 1 << 0,
	Stackable   = 1 << 1,
	Craftable   = 1 << 2,
	Dropable    = 1 << 3,
	Consumable  = 1 << 4,
	QuestItem   = 1 << 5,
	Expirable   = 1 << 6,
	Durable     = 1 << 7
};
ENUM_CLASS_FLAGS(EInventoryItemFlags)


