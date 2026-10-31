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
class UMounteaJsonObject;

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
	 * Constructs the item actions container from a Mountea JSON payload.
	 *
	 * @param Payload JSON payload conforming to the ActionsCreateRequest definition: "actions" (array of scriptable actions) and "context" (JSON object).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Item Actions Container")
	void ConstructFromActionsList(const UMounteaJsonObject* Payload);
	virtual void ConstructFromActionsList_Implementation(const UMounteaJsonObject* Payload) = 0;

	/**
	 * Adds an item action widget to the container.
	 *
	 * @param Payload JSON payload conforming to the ActionCreateRequest definition: "action" (scriptable action) and "context" (JSON object).
	 *
	 * @return Returns create Widget if action is successful
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Item Actions Container")
	UWidget* AddItemActionToContainer(const UMounteaJsonObject* Payload);
	virtual UWidget* AddItemActionToContainer_Implementation(const UMounteaJsonObject* Payload) = 0;

	/**
	 * Removes an item action widget from the container.
	 * 
	 * @param ItemActionWidget The widget to be removed from the container. Must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Item Actions Container")
	void RemoveItemActionFromContainer(UWidget* ItemActionWidget);
	virtual void RemoveItemActionFromContainer_Implementation(UWidget* ItemActionWidget) = 0;

	/**
	 * Clears all item action widgets from the container.
	 * 
	 * This function is typically called when the item actions are no longer needed or when the inventory is closed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Item Actions Container")
	void ClearItemActionsContainer();
	virtual void ClearItemActionsContainer_Implementation() = 0;

	/**
	 * Selects an item action widget from the container.
	 * 
	 * @param ItemActionWidget The widget to be selected. Must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Item Actions Container")
	void SelectItemAction(UWidget* ItemActionWidget);
	virtual void SelectItemAction_Implementation(UWidget* ItemActionWidget) = 0;

	/**
	 * Retrieves all item action widgets currently in the container.
	 * 
	 * @return An array of UWidget pointers representing the item action widgets in the container.
	 * Each widget must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Item Actions Container")
	TArray<UWidget*> GetItemActionsInContainer();
	virtual TArray<UWidget*> GetItemActionsInContainer_Implementation() const = 0;
};
