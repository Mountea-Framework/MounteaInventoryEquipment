// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemsGridWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemsGridWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for an advanced inventory items grid widget.
 * 
 * This interface allows interacting with the item grid in the UI by adding, removing,
 * swapping, and querying items in individual slots.
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemsGridWidgetInterface
{
	GENERATED_BODY()

public:
	/**
	 * Adds an item to the first available (empty) slot in the grid.
	 * 
	 * @param ItemId The unique identifier of the item to add.
	 * @return True if the item was successfully added; false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool AddItemToEmptySlot(const FGuid& ItemId);
	virtual bool AddItemToEmptySlot_Implementation(const FGuid& ItemId) = 0;

	/**
	 * Adds an item to a specific slot in the grid.
	 * 
	 * @param ItemId The unique identifier of the item to add.
	 * @param SlotIndex The index of the slot to place the item into.
	 * @return True if the item was successfully added; false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool AddItemToSlot(const FGuid& ItemId, const int32 SlotIndex);
	virtual bool AddItemToSlot_Implementation(const FGuid& ItemId, const int32 SlotIndex) = 0;

	/**
	 * Removes an item from the specified slot.
	 * 
	 * @param SlotIndex The index of the slot from which to remove the item.
	 * @return True if the item was successfully removed; false if the slot is empty or invalid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool RemoveItemFromSlot(const int32 SlotIndex);
	virtual bool RemoveItemFromSlot_Implementation(const int32 SlotIndex) = 0;

	/**
	 * Gets the item ID from a specific slot.
	 * 
	 * @param SlotIndex The index of the slot to query.
	 * @return The unique identifier (FGuid) of the item in the slot. 
	 *         If the slot is empty, returns an invalid FGuid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	FGuid GetItemInSlot(const int32 SlotIndex) const;
	virtual FGuid GetItemInSlot_Implementation(const int32 SlotIndex) const = 0;

	/**
	 * Swaps the items between two specific slots.
	 * 
	 * @param SlotIndex1 The index of the first slot.
	 * @param SlotIndex2 The index of the second slot.
	 * @return True if the swap was successful; false if either slot is invalid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool SwapItemsBetweenSlots(const int32 SlotIndex1, const int32 SlotIndex2);
	virtual bool SwapItemsBetweenSlots_Implementation(const int32 SlotIndex1, const int32 SlotIndex2) = 0;

	/**
	 * Clears all items from every slot in the grid.
	 * 
	 * This is useful for resetting or clearing the inventory grid entirely.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	void ClearAllSlots();
	virtual void ClearAllSlots_Implementation() = 0;

	/**
	 * Gets the total number of slots in the grid.
	 * 
	 * @return The total number of slots available in the inventory grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	int32 GetTotalSlots() const;
	virtual int32 GetTotalSlots_Implementation() const = 0;

	/**
	 * Checks if a specific slot is empty.
	 * 
	 * @param SlotIndex The index of the slot to check.
	 * @return True if the slot is empty; false if it contains an item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool IsSlotEmpty(const int32 SlotIndex) const;
	virtual bool IsSlotEmpty_Implementation(const int32 SlotIndex) const = 0;

	/**
	 * Gets the slot index that contains the specified item.
	 * 
	 * @param ItemId The unique identifier (FGuid) of the item to locate.
	 * @return The index of the slot that contains the item, or -1 if the item is not found.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	int32 GetSlotIndexByItem(const FGuid& ItemId) const;
	virtual int32 GetSlotIndexByItem_Implementation(const FGuid& ItemId) const = 0;

	/**
	 * Checks if a specific item exists anywhere in the grid.
	 * 
	 * @param ItemId The unique identifier of the item to check for.
	 * @return True if the item is found in the grid; false if it is not present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool IsItemInGrid(const FGuid& ItemId) const;
	virtual bool IsItemInGrid_Implementation(const FGuid& ItemId) const = 0;

	/**
	 * Retrieves the data of a specific inventory grid slot.
	 *
	 * This method returns information about a grid slot based on its index.
	 * It is used to fetch details such as the position of the slot and the item it contains.
	 *
	 * @param SlotIndex The index of the grid slot to retrieve data for.
	 * @return FMounteaInventoryGridSlot containing information about the specified slot.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	FMounteaInventoryGridSlot GetGridSlotData(const int32 SlotIndex) const;
	virtual FMounteaInventoryGridSlot GetGridSlotData_Implementation(const int32 SlotIndex) const = 0;

	/**
	 * Retrieves the data of all inventory grid slots.
	 *
	 * This method returns a set of FMounteaInventoryGridSlot, each representing an individual slot in the inventory grid.
	 * The data includes information such as the slot's position and the item it contains.
	 *
	 * @return A set of FMounteaInventoryGridSlot containing the data for all slots in the grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	TSet<FMounteaInventoryGridSlot> GetGridSlotsData() const;
	virtual TSet<FMounteaInventoryGridSlot> GetGridSlotsData_Implementation() const = 0;

	/**
	 * Finds the widget corresponding to the first empty slot in the grid.
	 *
	 * This method is used to retrieve the user interface widget tied to the first available empty slot,
	 * enabling interaction or customization of the slot in the inventory system.
	 *
	 * @return A UUserWidget pointer representing the first empty slot in the grid.
	 *         Returns nullptr if no empty slots are available.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	UUserWidget* FindEmptyWidgetSlot() const;
	virtual UUserWidget* FindEmptyWidgetSlot_Implementation() const = 0;

	/**
	 * Finds the index of the first available empty slot in the inventory grid.
	 *
	 * This method searches the inventory grid for an unused slot and returns its index.
	 *
	 * @return The index of the first empty slot, or -1 if no empty slots are available.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	int32 FindEmptySlotIndex(const FGuid& ItemId) const;
	virtual int32 FindEmptySlotIndex_Implementation(const FGuid& ItemId) const = 0;

	/**
	 * Retrieves the widget corresponding to a specific item slot in the grid.
	 *
	 * This function allows querying the UI widget associated with the specified slot index.
	 *
	 * @param SlotIndex The index of the slot to retrieve the widget for.
	 * @return The widget associated with the specified slot index, or nullptr if no widget exists for the given index.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	UUserWidget* GetItemSlotWidget(const int32 SlotIndex) const;
	virtual UUserWidget* GetItemSlotWidget_Implementation(const int32 SlotIndex) const = 0;

	/**
	 * Retrieves the user interface widget associated with an individual item slot in the grid.
	 *
	 * This method allows access to the UI representation of a specific item slot, useful for customization,
	 * updates, or interaction within the inventory system.
	 *
	 * @return The UUserWidget representing the item slot. Returns nullptr if no widget is associated.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	UUserWidget* GetItemWidgetInSlot(const int32 SlotIndex) const;
	virtual UUserWidget* GetItemWidgetInSlot_Implementation(const int32 SlotIndex) const = 0;

	/**
	 * Adds a new slot to the inventory grid widget.
	 *
	 * This function allows adding a slot with specified data to the grid, enabling
	 * customization and extension of the inventory UI.
	 *
	 * @param SlotData The data representing the slot to be added to the inventory grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	void AddSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void AddSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	/**
	 * Updates the item located in a specific slot of the inventory grid.
	 *
	 * This method attempts to update the item in the specified slot index by using the provided item ID.
	 * If a slot index is not specified, the default slot index (0) will be used.
	 *
	 * @param ItemId A globally unique identifier (GUID) for the item to be updated.
	 * @param SlotIndex The index of the slot in which the item is updated (default is 0).
	 * @return Returns true if the item was successfully updated, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid")
	bool UpdateItemInSlot(const FGuid& ItemId, const int32 SlotIndex = 0);
	virtual bool UpdateItemInSlot_Implementation(const FGuid& ItemId, const int32 SlotIndex = 0) = 0;
};
