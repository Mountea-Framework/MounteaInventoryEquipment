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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemWidgetInterface.generated.h"

struct FInventorySlot;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for widgets representing individual inventory items.
 *
 * This interface defines the contract for UI widgets that visually display
 * inventory items and react to changes in item state. Implementations are responsible for:
 *
 * - Displaying item metadata and quantity
 * - Applying highlights, states, and visual refresh logic
 * - Holding and exposing their associated inventory data
 * - Maintaining an association with the parent inventory slot widget
 *
 * This keeps UI responsibilities isolated from inventory logic, while still allowing
 * widgets to query and present relevant item details.
 *
 * @see FInventoryItem
 * @see FInventoryItemData
 * @see FInventorySlot
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Retrieves the logical data currently represented by this item widget.
	 *
	 * Implementations should always return the latest data that the UI reflects.
	 *
	 * @return A copy of the item’s data structure describing quantity and contained item state.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	FInventoryItemData GetInventoryData() const;
	virtual FInventoryItemData GetInventoryData_Implementation() const = 0;

	/**
	 * Assigns new inventory data to this widget.
	 *
	 * Implementations should update visuals immediately or trigger an internal refresh.
	 *
	 * @param InventoryData Data describing the item and its quantity.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetInventoryData(const FInventoryItemData& InventoryData);
	virtual void SetInventoryData_Implementation(const FInventoryItemData& InventoryData) = 0;

	/**
	 * Gets the GUID of the item currently bound to this widget.
	 *
	 * @return GUID identifying the item, or invalid GUID if no valid item exists.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	FGuid GetInventoryItemId() const;
	virtual FGuid GetInventoryItemId_Implementation() const = 0;

	/**
	 * Forces the widget to visually refresh itself.
	 *
	 * Typical use cases:
	 * - Quantity change
	 * - State updates (cooldowns, durability, etc.)
	 *
	 * @param Quantity The current quantity to visually reflect.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void RefreshItemWidget(const int32 Quantity);
	virtual void RefreshItemWidget_Implementation(const int32 Quantity) = 0;

	/**
	 * Returns the UI widget representing the parent slot that owns this item widget.
	 *
	 * @return Widget representing the slot that contains this item instance.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	UUserWidget* GetParentSlot() const;
	virtual UUserWidget* GetParentSlot_Implementation() const = 0;
	
	/**
	 * Assigns the parent slot that visually contains this item widget.
	 *
	 * Intended to be called when this item is inserted or moved between slots.
	 *
	 * @param ParentSlot Widget representing the inventory slot owning this item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetParentSlot(UUserWidget* ParentSlot);
	virtual void SetParentSlot_Implementation(UUserWidget* ParentSlot) = 0;

	/**
	 * Toggles visual highlighting for this item widget.
	 *
	 * Useful for:
	 * - Selection feedback
	 * - Drag-and-drop feedback
	 * - Interaction state changes
	 *
	 * @param bIsSelected Whether the widget should be rendered as highlighted.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void HighlightItem(const bool bIsSelected = false);
	virtual void HighlightItem_Implementation(const bool bIsSelected = false) = 0;
};
