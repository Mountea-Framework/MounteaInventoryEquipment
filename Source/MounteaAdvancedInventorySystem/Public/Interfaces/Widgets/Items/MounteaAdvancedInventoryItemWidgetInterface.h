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
#include "MounteaAdvancedInventoryItemWidgetInterface.generated.h"

struct FInventorySlot;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryItemWidgetInterface manages individual inventory item widget presentation.
 * Item widget interfaces handle item identification, visual refresh, highlighting, and slot association
 * for individual item representations within inventory interfaces.
 *
 * @see [Item Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemWidgets)
 * @see FInventoryItem
 * @see FInventorySlot
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param ItemGuid 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetInventoryItemId(const FGuid& ItemGuid);
	virtual void SetInventoryItemId_Implementation(const FGuid& ItemGuid) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	FGuid GetInventoryItemId() const;
	virtual FGuid GetInventoryItemId_Implementation() const = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void RefreshItemWidget(const int32 Quantity);
	virtual void RefreshItemWidget_Implementation(const int32 Quantity) = 0;

	/**
	 * Retrieves the data associated with the inventory slot.
	 * @return The inventory slot data.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	FInventorySlot GetSlotData() const;
	virtual FInventorySlot GetSlotData_Implementation() const = 0;
	
	/**
	 *
	 * @param ParentSlot The parent slot to be assigned to the inventory item widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void SetParentSlot(UUserWidget* ParentSlot);
	virtual void SetParentSlot_Implementation(UUserWidget* ParentSlot) = 0;

	/**
	 * Highlights or un-highlights an inventory item within the UI by interacting with the provided widget.
	 *
	 * @param bIsSelected A boolean indicating whether to highlight (true) or un-highlight (false) the item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void HighlightItem(const bool bIsSelected = false);
	virtual void HighlightItem_Implementation(const bool bIsSelected = false) = 0;
};
