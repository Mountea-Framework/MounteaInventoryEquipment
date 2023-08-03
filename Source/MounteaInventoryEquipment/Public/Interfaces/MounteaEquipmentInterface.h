// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaEquipmentInterface.generated.h"

struct FMounteaEquipmentSlotData;
struct FMounteaEquipmentSlots;
class UMounteaInventoryItemBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
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

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool EquipItem(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex = 0);
	virtual bool EquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex = 0) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool UnEquipItem(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex = 0);
	virtual bool UnEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex = 0) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool IsItemEquipped(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex = 0);
	virtual bool IsItemEquipped_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const int32 OptionalIndex = 0) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FMounteaEquipmentSlotData> GetAllSlots() const;
	virtual TArray<FMounteaEquipmentSlotData> GetAllSlots_Implementation() const = 0;
};
