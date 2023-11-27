// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Helpers/MounteaEquipmentHelpers.h"
#include "UObject/Interface.h"
#include "MounteaEquipmentInterface.generated.h"

class UMounteaBaseUserWidget;
class UMounteaInstancedItem;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUpdated, const FInventoryUpdateResult&, UpdateContext);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FText FindSlotForItem(const UMounteaInstancedItem* Item) const;
	virtual FText FindSlotForItem_Implementation(const UMounteaInstancedItem* Item) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	int32 FindSlotByID(const FText& SlotID) const;
	virtual int32 FindSlotByID_Implementation(const FText& SlotID) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FInventoryUpdateResult EquipItem(UMounteaInstancedItem* ItemToEquip, const FText& SlotID);
	virtual FInventoryUpdateResult EquipItem_Implementation(UMounteaInstancedItem* ItemToEquip,const FText& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FInventoryUpdateResult UnEquipItem(UMounteaInstancedItem* ItemToEquip, const FText& SlotID);
	virtual FInventoryUpdateResult UnEquipItem_Implementation(UMounteaInstancedItem* ItemToEquip, const FText& SlotID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool IsItemEquipped(const UMounteaInstancedItem* ItemToEquip, const FText& SlotID) const;
	virtual bool IsItemEquipped_Implementation(const UMounteaInstancedItem* ItemToEquip, const FText& SlotID) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	TArray<FEquipmentSlot> GetAllSlots() const;
	virtual TArray<FEquipmentSlot> GetAllSlots_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool CanEquipItem(const UMounteaInstancedItem* ItemToEquip) const;
	virtual bool CanEquipItem_Implementation(const UMounteaInstancedItem* ItemToEquip) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool CanUnEquipItem(const UMounteaInstancedItem* ItemToUnequip) const;
	virtual bool CanUnEquipItem_Implementation(const UMounteaInstancedItem* ItemToUnequip) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	UMounteaBaseUserWidget* GetEquipmentUI() const;
	virtual UMounteaBaseUserWidget* GetEquipmentUI_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool SetEquipmentUI(UMounteaBaseUserWidget* NewUI);
	virtual bool SetEquipmentUI_Implementation(UMounteaBaseUserWidget* NewUI) = 0;

	/**
	 * Checks if the Equipment has authority within the networked game.
	 * @return True if the component has authority, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool DoesHaveAuthority() const;
	virtual bool DoesHaveAuthority_Implementation() const = 0;
	
public:

	virtual FOnEquipmentUpdated& GetEquipmentUpdatedHandle() = 0;
	virtual FOnEquipmentUpdated& GetSlotEquippedHandle() = 0;
	virtual FOnEquipmentUpdated& GetSlotUnEquippedHandle() = 0;
};
