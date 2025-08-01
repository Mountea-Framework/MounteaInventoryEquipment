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
#include "MounteaAdvancedInventoryItemActionsContainerWidgetInterface.generated.h"

class UWidget;
class UMounteaInventoryItemAction;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemActionsContainerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryItemActionsContainerWidgetInterface manages container widgets for item actions.
 * Item actions container interfaces handle the grouping and presentation of available item actions
 * within action panels and context menus for inventory item interaction.
 *
 * @see [Item Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryItemActionWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemActionsContainerWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * Sets the parent item widget for this item actions container.
	 * 
	 * @param ParentItemWidget The parent widget that owns this item actions container.
	 * 
	 * This function is typically called to establish the relationship between the item actions container
	 * and the item widget it is associated with, allowing for proper context and interaction.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	void SetParentItemWidget(UWidget* ParentItemWidget);
	virtual void SetParentItemWidget_Implementation(UWidget* ParentItemWidget) = 0;

	/**
	 * Constructs the item actions container from a list of item action classes.
	 * 
	 * @param ItemActionsList An array of TSoftClassPtr<UObject> representing the item actions to be added to the container.
	 * 
	 * Each class must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	void ConstructFromActionsList(const TArray<TSoftClassPtr<UObject>>& ItemActionsList);
	virtual void ConstructFromActionsList_Implementation(const TArray<TSoftClassPtr<UObject>>& ItemActionsList) = 0;
	
	/**
	 * Adds an item action widget to the container.
	 * 
	 * @param ItemActionWidget The widget to be added to the container. Must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	void AddItemActionToContainer(UUserWidget* ItemActionWidget);
	virtual void AddItemActionToContainer_Implementation(UUserWidget* ItemActionWidget) = 0;

	/**
	 * Removes an item action widget from the container.
	 * 
	 * @param ItemActionWidget The widget to be removed from the container. Must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	void RemoveItemActionFromContainer(UUserWidget* ItemActionWidget);
	virtual void RemoveItemActionFromContainer_Implementation(UUserWidget* ItemActionWidget) = 0;

	/**
	 * Clears all item action widgets from the container.
	 * 
	 * This function is typically called when the item actions are no longer needed or when the inventory is closed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	void ClearItemActionsContainer();
	virtual void ClearItemActionsContainer_Implementation() = 0;

	/**
	 * Selects an item action widget from the container.
	 * 
	 * @param ItemActionWidget The widget to be selected. Must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	void SelectItemAction(UUserWidget* ItemActionWidget);
	virtual void SelectItemAction_Implementation(UUserWidget* ItemActionWidget) = 0;

	/**
	 * Retrieves all item action widgets currently in the container.
	 * 
	 * @return An array of UUserWidget pointers representing the item action widgets in the container.
	 * Each widget must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container")
	TArray<UUserWidget*> GetItemActionsInContainer();
	virtual TArray<UUserWidget*> GetItemActionsInContainer_Implementation() const = 0;
};
