// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MounteaInventoryFlagsLibrary.generated.h"

UENUM()
enum class EInventoryItemFlags : uint32
{
	None        = 0        UMETA(DisplayName="None"),
	Tradeable   = 1 << 0   UMETA(DisplayName="Tradeable"),
	Stackable   = 1 << 1   UMETA(DisplayName="Stackable"),
	Craftable   = 1 << 2   UMETA(DisplayName="Craftable"),
	Dropable    = 1 << 3   UMETA(DisplayName="Dropable"),
	Consumable  = 1 << 4   UMETA(DisplayName="Consumable"),
	QuestItem   = 1 << 5   UMETA(DisplayName="Quest Item"),
	Expirable   = 1 << 6   UMETA(DisplayName="Expirable"),
	Durable     = 1 << 7   UMETA(DisplayName="Durable")
};
ENUM_CLASS_FLAGS(EInventoryItemFlags)

