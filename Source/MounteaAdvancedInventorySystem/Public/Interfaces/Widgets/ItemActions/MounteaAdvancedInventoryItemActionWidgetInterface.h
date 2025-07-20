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
#include "MounteaAdvancedInventoryItemActionWidgetInterface.generated.h"

class UMounteaInventoryItemAction;
class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemActionWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryItemActionWidgetInterface defines individual item action widget functionality.
 * Item action interfaces manage individual actionable buttons and controls for inventory item operations
 * such as use, equip, drop, and other context-specific item interactions.
 *
 * @see [Item Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryItemActionsContainerWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemActionWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * Initializes the item action widget with the parent UI and item action data.
	 * 
	 * @param ParentUI The parent UI interface that owns this item action widget.
	 * @param ItemAction The item action data associated with this widget.
	 * @param ItemId The unique identifier for the item this action is associated with.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	void InitializeItemAction(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, UMounteaInventoryItemAction* ItemAction, const FGuid& ItemId);
	virtual void InitializeItemAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, UMounteaInventoryItemAction* ItemAction, const FGuid& ItemId) = 0;

	/**
	 * Retrieves the item action associated with this widget.
	 * 
	 * @return The item action data associated with this widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	bool IsActionEnabled() const;
	virtual bool IsActionEnabled_Implementation() const = 0;

	/**
	 * Checks if the item action is valid for execution.
	 * 
	 * @return True if the action is valid, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	bool IsActionValid() const;
	virtual bool IsActionValid_Implementation() const = 0;

	/**
	 * Executes the item action when triggered by the user.
	 * 
	 * This function is typically called when the user clicks or interacts with the item action button.
	 * It should handle the logic for performing the action, such as using, equipping, or dropping the item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	void ExecuteItemAction();
	virtual void ExecuteItemAction_Implementation() = 0;
};
