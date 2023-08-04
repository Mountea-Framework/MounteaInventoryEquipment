// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryItem.h"
#include "UObject/Interface.h"
#include "MounteaEquipmentInterface.generated.h"

class UMounteaEquipmentSlot;
struct FMounteaEquipmentSlotData;
struct FMounteaEquipmentSlots;
class UMounteaInventoryItemBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUpdated, const FString&, UpdateContext);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotUpdated,  const class UMounteaInventoryItemBase*, Item, const FString&, UpdateContext);

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
	FString FindSlotForItem(const UMounteaInventoryItemBase* Item) const;
	virtual FString FindSlotForItem_Implementation(const UMounteaInventoryItemBase* Item) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaEquipmentSlot* FindSlotByID(const FString& SlotID) const;
	virtual UMounteaEquipmentSlot* FindSlotByID_Implementation(const FString& SlotID) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool EquipItem(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	virtual bool EquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip,const FString& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool UnEquipItem(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	virtual bool UnEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool IsItemEquipped(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID);
	virtual bool IsItemEquipped_Implementation(const UMounteaInventoryItemBase* ItemToEquip, const FString& SlotID) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FMounteaEquipmentSlotData> GetAllSlots() const;
	virtual TArray<FMounteaEquipmentSlotData> GetAllSlots_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanEquipItem(const UMounteaInventoryItemBase* ItemToEquip) const;
	virtual bool CanEquipItem_Implementation(const UMounteaInventoryItemBase* ItemToEquip) const = 0;
public:

	virtual FOnEquipmentUpdated& GetEquipmentUpdatedHandle() = 0;
	virtual FOnSlotUpdated& GetSlotEquippedHandle() = 0;
	virtual FOnSlotUpdated& GetSlotUnEquippedHandle() = 0;
};
