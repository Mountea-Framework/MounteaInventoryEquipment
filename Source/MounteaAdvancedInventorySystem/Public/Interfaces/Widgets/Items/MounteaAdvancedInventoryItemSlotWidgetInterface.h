// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemSlotWidgetInterface.generated.h"

struct FMounteaInventoryGridSlot;
class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemSlotWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemSlotWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	void SetOwningInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI) = 0;

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
	 * Retrieves the data associated with the inventory grid slot.
	 * This is optional information not all Slots need have (non-grid
	 * based inventory layouts won't have any grid slot data).
	 *
	 * @return The inventory grid slot data.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots")
	FMounteaInventoryGridSlot GetSlotData() const;
	virtual FMounteaInventoryGridSlot GetSlotData_Implementation() const = 0;

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
};
