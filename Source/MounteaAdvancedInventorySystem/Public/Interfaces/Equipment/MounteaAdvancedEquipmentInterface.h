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
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedEquipmentInterface.generated.h"

struct FAttachmentContainer;
struct FAttachmentSlot;
struct FInventoryItem;

class UMounteaInventoryItemTemplate;

enum class EEquipmentItemState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipmentItemActivated, UObject*, EquipmentItem);

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedEquipmentInterface provides specialized functionality for equipment management systems.
 * Equipment interfaces extend attachment containers with equipment-specific operations, stat modifications,
 * and character integration for comprehensive equipment systems within the inventory framework.
 *
 * @see [Equipment Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/EquipmentSystem)
 * @see UMounteaEquipmentComponent
 * @see IMounteaAdvancedAttachmentContainerInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedEquipmentInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Equips the specified inventory item using the equipment system's internal slot resolution.
	 *
	 * @param ItemDefinition The inventory item definition to equip.
	 * @return Spawned equipment actor if equipping succeeds, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	AActor* EquipItem(const FInventoryItem& ItemDefinition);
	virtual AActor* EquipItem_Implementation(const FInventoryItem& ItemDefinition) = 0; 
	
	/**
	 * Equips the specified inventory item into a designated equipment slot.
	 *
	 * @param SlotId Name of the equipment slot where the item should be equipped.
	 * @param ItemDefinition The inventory item definition to equip.
	 * @return Spawned equipment actor if equipping succeeds, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	AActor* EquipItemToSlot(const FName& SlotId, const FInventoryItem& ItemDefinition);
	virtual AActor* EquipItemToSlot_Implementation(const FName& SlotId, const FInventoryItem& ItemDefinition) = 0;
	
	/**
	 * Unequips the specified inventory item.
	 *
	 * @param ItemDefinition The inventory item definition to unequip.
	 * @param bUseFallbackSlot If true, fallback slot logic is allowed during unequip.
	 * @return True if the item was successfully unequipped, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool UnequipItem(const FInventoryItem& ItemDefinition, bool bUseFallbackSlot = false);
	virtual bool UnequipItem_Implementation(const FInventoryItem& ItemDefinition, bool bUseFallbackSlot = false) = 0;
	
	/**
	 * Unequips the item currently occupying the specified slot.
	 *
	 * @param SlotId Name of the equipment slot to unequip.
	 * @param bUseFallbackSlot If true, fallback slot logic is allowed during unequip.
	 * @return True if an item was successfully unequipped from the slot, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool UnequipItemFromSlot(const FName& SlotId, bool bUseFallbackSlot = false);
	virtual bool UnequipItemFromSlot_Implementation(const FName& SlotId, bool bUseFallbackSlot = false) = 0;
	
	/**
	 * Checks whether the specified inventory item is currently equipped.
	 *
	 * @param ItemDefinition The inventory item definition to check.
	 * @return True if the item is equipped, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool IsEquipmentItemEquipped(const FInventoryItem& ItemDefinition) const;
	virtual bool IsEquipmentItemEquipped_Implementation(const FInventoryItem& ItemDefinition) const = 0;
	
	/**
	 * Checks whether the specified inventory item is equipped in a particular slot.
	 *
	 * @param ItemDefinition The inventory item definition to check.
	 * @param SlotName The slot name to validate.
	 * @return True if the item is equipped in the specified slot, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool IsEquipmentItemEquippedInSlot(const FInventoryItem& ItemDefinition, const FName& SlotName) const;
	virtual bool IsEquipmentItemEquippedInSlot_Implementation(const FInventoryItem& ItemDefinition, const FName& SlotName) const = 0;

	/**
	 * Activates the specified equipped item and transitions it from Equipped to Active state.
	 * Activation may trigger item-specific animation flow depending on implementation.
	 *
	 * @param ItemDefinition The inventory item definition to activate.
	 * @param TargetSlotId Optional target slot used by activation flow.
	 * @return True if activation was successfully initiated, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool ActivateEquipmentItem(const FInventoryItem& ItemDefinition, const FName& TargetSlotId);
	virtual bool ActivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId) = 0;

	/**
	 * Activates a quick-use item from the specified slot.
	 *
	 * @param SlotId The slot from which the quick-use item will be activated.
	 * @param TargetSlotId Optional target slot used by quick-use activation flow.
	 * @return True if activation succeeds, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool ActivateQuickUseItem(const FName& SlotId, const FName& TargetSlotId);
	virtual bool ActivateQuickUseItem_Implementation(const FName& SlotId, const FName& TargetSlotId) { return false; }

	/**
	 * Deactivates the specified active item and transitions it back toward Equipped/storage state.
	 *
	 * @param ItemDefinition The inventory item definition to deactivate.
	 * @param TargetSlotId Optional target slot used by deactivation flow.
	 * @return True if deactivation was successfully initiated, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool DeactivateEquipmentItem(const FInventoryItem& ItemDefinition, const FName& TargetSlotId);
	virtual bool DeactivateEquipmentItem_Implementation(const FInventoryItem& ItemDefinition, const FName& TargetSlotId) = 0;

	/**
	 * Commits pending equipment transition requested by animation notify flow.
	 *
	 * @return True if notify handling succeeded, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool AnimAttachItem();
	virtual bool AnimAttachItem_Implementation() = 0;

	/**
	 * Commits quick-use animation notify workflow.
	 *
	 * @return True if notify handling succeeded, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Equipment")
	bool AnimQuickItemUsed();
	virtual bool AnimQuickItemUsed_Implementation() { return false; }

	/**
	 * Retrieves deferred activation payload when activation is waiting for animation/event completion.
	 *
	 * @param OutPendingActivation Output payload for pending activation.
	 * @return True if pending activation data is available.
	 */
	virtual bool TryGetPendingEquipmentActivation(FPendingEquipmentActivation& OutPendingActivation) const
	{
		OutPendingActivation = FPendingEquipmentActivation();
		return false;
	}
};

