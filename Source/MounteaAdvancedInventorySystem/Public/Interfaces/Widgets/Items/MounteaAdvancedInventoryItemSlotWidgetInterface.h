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
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemSlotWidgetInterface.generated.h"

struct FInventorySlot;
struct FMounteaInventoryGridSlot;
class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemSlotWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryItemSlotWidgetInterface defines individual inventory slot widget functionality.
 * Item slot interfaces manage individual slot presentation, item containment, tooltip generation,
 * and slot-specific operations for individual inventory slot widgets.
 *
 * @see [Item Slots](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see FInventorySlot
 * @see FMounteaInventoryGridSlot
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemSlotWidgetInterface
{
	GENERATED_BODY()

public:
	/**
	 *	Gets the tooltip text from the Slot based on the item it contains.
	 * 
	 * @return String tooltip of the Slot.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	FString GetSlotTooltip() const;
	virtual FString GetSlotTooltipText_Implementation() const = 0;

	/**
	 * Sets the parent widget that contains the slot.
	 *
	 * @param ParentSlotsWrapper The parent widget that contains the slots.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void SetParentSlotsWrapper(UUserWidget* ParentSlotsWrapper);
	virtual void SetParentSlotsWrapper_Implementation(UUserWidget* ParentSlotsWrapper) = 0;

	/**
	 * Stores the slot data for inventory purposes.
	 * 
	 * @param SlotData The slot data to be stored.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void StoreBasicSlotData(const FInventorySlot& SlotData);
	virtual void StoreBasicSlotData_Implementation(const FInventorySlot& SlotData) = 0;

	/**
	 * Stores the grid slot data for inventory purposes.
	 * This is optional information not all Slots need have (non-grid
	 * based inventory layouts won't have any grid slot data).
	 * 
	 * @param SlotData The grid slot data to be stored.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void StoreGridSlotData(const FMounteaInventoryGridSlot& SlotData);
	virtual void StoreGridSlotData_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;
	
	/**
	 * Retrieves the data associated with the inventory slot.
	 *
	 * @return The inventory slot data.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	FInventorySlot GetSlotData() const;
	virtual FInventorySlot GetSlotData_Implementation() const = 0;

	/**
	 * Retrieves the data associated with the inventory grid slot.
	 * This is optional information not all Slots need have (non-grid
	 * based inventory layouts won't have any grid slot data).
	 *
	 * @return The inventory grid slot data.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	FMounteaInventoryGridSlot GetGridSlotData() const;
	virtual FMounteaInventoryGridSlot GetGridSlotData_Implementation() const = 0;

	/**
	 * Adds an item to the slot using the provided item ID.
	 *
	 * @param ItemId The unique identifier of the item to be added to the slot.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void AddItemToSlot(const FGuid& ItemId);
	virtual void AddItemToSlot_Implementation(const FGuid& ItemId) = 0;

	/**
	 * Removes an item from the slot using the provided item ID.
	 *
	 * @param ItemId The unique identifier of the item to be removed from the slot.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void RemoveItemFromSlot(const FGuid& ItemId);
	virtual void RemoveItemFromSlot_Implementation(const FGuid& ItemId) = 0;

	/**
	 * Checks whether the slot is empty.
	 *
	 * @return True if the slot is empty, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	bool IsSlotEmpty() const;
	virtual bool IsSlotEmpty_Implementation() const = 0;

	/**
	 * Retrieves the item widget currently in the inventory slot.
	 *
	 * @return The item widget in the slot, or nullptr if no widget is present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	UUserWidget* GetItemWidgetInSlot() const;
	virtual UUserWidget* GetItemWidgetInSlot_Implementation() const = 0;

	/**
	 * Handles the logic for selecting an item in the corresponding inventory slot.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void SelectItemInSlot();
	virtual void SelectItemInSlot_Implementation() = 0;
};
