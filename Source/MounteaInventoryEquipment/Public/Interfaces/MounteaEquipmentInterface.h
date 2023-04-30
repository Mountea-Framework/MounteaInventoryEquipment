// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaEquipmentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMounteaEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Mountea Equipment Interface.
 *
 * Provides functionality to manage Equipment, for example:
 * * Equip Item
 * * Unequip Item
 * * Move item to Equipment slot
 *
 * Equipment is standalone, however, works best with `IMounteaInventoryInterface` as source of Items.
 *
 * Equipment is player-specific and thus should be attached to Player Controller.
 * If player equipment is tied to Player pawn (if player dies, his loot stays on his corpse), then use Player Pawn instead.
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaEquipmentInterface
{
	GENERATED_BODY()
	
};
