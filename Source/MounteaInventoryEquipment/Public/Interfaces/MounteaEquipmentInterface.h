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

	/**
	 * Retrieves the actor that owns this equipment. For components it is common to return Owner, for Actors to return themselves.
	 * @return The owning actor of this equipment.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	/**
	 * Finds a suitable slot for the given item based on the equipment's rules and configuration.
	 * @param Item The item for which a suitable slot needs to be found.
	 * @return The ID of the slot that can accommodate the item, or None if none is found.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FGameplayTag FindSlotForItem(const UMounteaInstancedItem* Item) const;
	virtual FGameplayTag FindSlotForItem_Implementation(const UMounteaInstancedItem* Item) const = 0;

	/**
	 * Finds a slot by its ID within the equipment.
	 * @param SlotID The ID of the slot to find.
	 * @return The index of the slot if found, or -1 if not found.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	int32 FindSlotByID(const FGameplayTag& SlotID) const;
	virtual int32 FindSlotByID_Implementation(const FGameplayTag& SlotID) const = 0;

	/**
	 * Equips an item to the specified slot, updating the equipment state accordingly.
	 * @param ItemToEquip The item to equip.
	 * @param SlotID The ID of the slot where the item is to be equipped.
	 * @return A structure containing details about the outcome of the equip process.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FInventoryUpdateResult EquipItem(UMounteaInstancedItem* ItemToEquip, const FGameplayTag& SlotID);
	virtual FInventoryUpdateResult EquipItem_Implementation(UMounteaInstancedItem* ItemToEquip,const FGameplayTag& SlotID) = 0;

	/**
	 * Unequips an item from the specified slot, updating the equipment state accordingly.
	 * @param ItemToEquip The item to unequip.
	 * @param SlotID The ID of the slot from which the item is to be unequipped.
	 * @return A structure containing details about the outcome of the unequip process.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	FInventoryUpdateResult UnEquipItem(UMounteaInstancedItem* ItemToEquip, const FGameplayTag& SlotID);
	virtual FInventoryUpdateResult UnEquipItem_Implementation(UMounteaInstancedItem* ItemToEquip, const FGameplayTag& SlotID) = 0;

	/**
	 * Checks if a specific item is currently equipped in a specified slot.
	 * @param ItemToEquip The item to check if it's equipped.
	 * @param SlotID The ID of the slot to check.
	 * @return True if the item is equipped in the specified slot, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool IsItemEquipped(UMounteaInstancedItem* ItemToEquip, const FGameplayTag& SlotID) const;
	virtual bool IsItemEquipped_Implementation(UMounteaInstancedItem* ItemToEquip, const FGameplayTag& SlotID) const = 0;

	/**
	 * Retrieves all the equipment slots available.
	 * @return An array of all equipment slots.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	TArray<FEquipmentSlot> GetAllSlots() const;
	virtual TArray<FEquipmentSlot> GetAllSlots_Implementation() const = 0;

	/**
	 * Checks if a specific item can be equipped.
	 * @param ItemToEquip The item to check.
	 * @return True if the item can be equipped, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool CanEquipItem(UMounteaInstancedItem* ItemToEquip) const;
	virtual bool CanEquipItem_Implementation(UMounteaInstancedItem* ItemToEquip) const = 0;

	/**
	 * Checks if a specific item can be unequipped.
	 * @param ItemToUnequip The item to check.
	 * @return True if the item can be unequipped, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	bool CanUnEquipItem(UMounteaInstancedItem* ItemToUnequip) const;
	virtual bool CanUnEquipItem_Implementation(UMounteaInstancedItem* ItemToUnequip) const = 0;

	/**
	 * Sets the UI widget for the equipment interface.
	 * @param NewUI The new UI widget to set.
	 * @return True if the UI was successfully set, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	UMounteaBaseUserWidget* GetEquipmentUI() const;
	virtual UMounteaBaseUserWidget* GetEquipmentUI_Implementation() const = 0;

	/**
	 * Retrieves the class type of the user interface associated with this equipment.
	 * This function returns the class type of the UI widget used for displaying and interacting with the equipment.
	 * 
	 * @return The class type of the equipment UI.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	TSubclassOf<UMounteaBaseUserWidget> GetEquipmentUIClass() const;
	virtual TSubclassOf<UMounteaBaseUserWidget> GetEquipmentUIClass_Implementation() const = 0;

	/**
	 * Sets the class type for the equipment's user interface.
	 * This function allows for dynamically changing the UI widget used for the equipment interface.
	 * It can be used to customize or upgrade the equipment UI according to game requirements.
	 * New class must implement `MounteaEquipmentWBPInterface`.
	 * 
	 * @param NewWBPClass The new widget blueprint class to set as the equipment UI class.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Equipment")
	void SetEquipmentUIClass(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentWBPInterface")) TSubclassOf<UMounteaBaseUserWidget> NewWBPClass);
	virtual void SetEquipmentUIClass_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentWBPInterface")) TSubclassOf<UMounteaBaseUserWidget> NewWBPClass) = 0;
	
	/**
	 * Retrieves the UI widget associated with the equipment interface.
	 * @return The UI widget of the equipment interface.
	 */
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
