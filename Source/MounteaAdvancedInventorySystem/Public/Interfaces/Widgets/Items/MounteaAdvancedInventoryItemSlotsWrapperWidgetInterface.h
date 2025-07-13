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
#include "MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface manages container widgets for item slots.
 * Item slots wrapper interfaces handle slot collection management, item addition/removal,
 * and slot selection coordination for inventory slot container widgets.
 *
 * @see [Item Slots](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryItemSlotWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * 
	 * @param ItemId 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void AddItem(const FGuid& ItemId);
	virtual void AddItem_Implementation(const FGuid& ItemId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void UpdateItem(const FGuid& ItemId, const int32 OptionalItemSlot = INDEX_NONE);
	virtual void UpdateItem_Implementation(const FGuid& ItemId, const int32 OptionalItemSlot = INDEX_NONE) = 0;

	/**
	 *
	 * @param ItemId Unique identifier of the item to be removed.
	 * @param Quantity The number of items to remove. If set to -1, all items with the given ID will be removed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void RemoveItem(const FGuid& ItemId, const int32 Quantity = -1);
	virtual void RemoveItem_Implementation(const FGuid& ItemId, const int32 Quantity = -1) = 0;

	/**
	 * Retrieves the currently selected item widget.
	 *
	 * @return Returns a pointer to the selected UUserWidget instance.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	UUserWidget* GetSelectedItemWidget() const;
	virtual UUserWidget* GetSelectedItemWidget_Implementation() const = 0;

	/**
	 * Attempts to set Item Widget as Selected one.
	 *
	 * @param NewSelectedItemWidget The widget to set as the currently selected item widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void SetSelectedItemWidget(UUserWidget* NewSelectedItemWidget);
	virtual void SetSelectedItemWidget_Implementation(UUserWidget* NewSelectedItemWidget) = 0;

};
