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
#include "MounteaInventoryItemAction.h"
#include "MounteaSelectableInventoryItemAction.generated.h"

class UMounteaCallbackInventoryItemAction;

#define LOCTEXT_NAMESPACE "MounteaInventoryItemActionData"

/**
 * UMounteaSelectableInventoryItemAction provides lightweight inventory actions intended purely for
 * local "UI-level" operations.
 *
 * Selectable Actions are executed directly inside the UI layer and should only handle presentation and
 * interaction flow — for example opening a modal window to select a quantity (eg. when consuming an item),
 * toggling an item as Favorite, showing a confirmation dialog, or previewing action details.
 *
 * ⚠ These actions must never perform gameplay logic or cross-system changes by themselves.
 *
 * If the action requires any execution logic (Inventory updates, Equipment changes, GAS interaction, etc.),
 * use the "CallbackAction" property, which defines the "Callable" sibling of this class:
 * "UMounteaCallbackInventoryItemAction".
 *
 * Example workflow:
 * - Selectable Action requests "Consume" and spawns a modal window to pick quantity
 * - Once confirmed, Selectable Action forwards the request to its "CallbackAction"
 * - Callback Action performs the actual Inventory System operation and any other runtime logic
 *
 * @see [Simple Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemActions)
 * @see UMounteaCallbackInventoryItemAction
 * @see UMounteaInventoryItemAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, Blueprintable, 
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Selectable Item Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaSelectableInventoryItemAction : public UMounteaInventoryItemAction
{
	GENERATED_BODY()
	
public:
	
	UMounteaSelectableInventoryItemAction() :
		ActionDisplayName(LOCTEXT("DefaultActionName", "Default Action")),
		ActionDescription(LOCTEXT("DefaultActionDescription", "A basic inventory item action")),
		ActionPriority(0),
		InventoryItemActionCallback(0)
	{};
	
protected:
	
	/**
	 * Display name of the action shown in the user interface.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FText ActionDisplayName;

	/**
	 * Brief description explaining what this action does.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FText ActionDescription;
	
	/** 
	 * Gameplay tag identifying the preferred UI Input to trigger this action. 
	 * Limited to "Input" category only!
	 * 
	 * If more than 1 is specified, the list assumes first one is most preferred and last one least.
	 * Fighting for inputs should NOT happen!
	 * If more than 1 Item Actions end up with same preferred keys, then `ActionPriority` is what decides the order
	 * of selection.
	 * If you run out of UI Inputs, then the Item Action will NOT be displayed, unless you implement your own logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		meta=(Categories="Input,Mountea_Inventory.Input"),
		meta=(TitleProperty="ActionTag"),
		meta=(NoResetToDefault))
	FGameplayTagContainer PreferredSelectionKeys;

	/**
	 * Priority value used for sorting actions in UI elements.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	int32 ActionPriority = INDEX_NONE;
	
	/**
	 * Callback type for this action, used to determine how the action is processed.
	 * This can be used to trigger different behaviors based on the action type.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemActionCallback"))
	uint8 InventoryItemActionCallback;

	/**
	 * Action which will be called if Inventory Item Action is required.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		Instanced)
	TObjectPtr<UMounteaCallbackInventoryItemAction> CallbackAction;
	
	/**
	 * List of tags which are disabling the action.
	 * If Item has `QuestItem` tag (temp. quest item), then `Use` would be disabled to avoid soft-locking of the quest.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration")
	FGameplayTagContainer ItemActionBlockingTags;
	
public:
	
	/**
	 * Determines whether this action should be visible in the UI for the given item.
	 * 
	 * @param TargetItem The inventory item to check visibility for.
	 * 
	 * @return True if the action should be shown in the user interface, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool IsActionVisible(const FInventoryItem& TargetItem) const;
	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const;
	
	/**
	 * Gets the callback type for this inventory item action.
	 * 
	 * This defines how the action interacts with the inventory system and UI.
	 * 
	 * @return The callback type indicating how this action should be processed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	EInventoryItemActionCallback GetInventoryItemActionCallback() const;
	virtual EInventoryItemActionCallback GetInventoryItemActionCallback_Implementation() const;
	
	/**
	 * Adds a specified action flag to an inventory item action.
	 * Designed to modify the behavior of item actions by appending a predefined flag.
	 *
	 * @param FlagToAdd The action flag to be added to the item action.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool AddActionFlag(const EInventoryItemActionCallback FlagToAdd);
	virtual bool AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd);
	
	/**
	 * Removes a specific action flag from the item.
	 * Allows modification of action flags associated with inventory items.
	 *
	 * @param FlagToRemove The flag to be removed from the item's action flags.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool RemoveActionFlag(const EInventoryItemActionCallback FlagToRemove);
	virtual bool RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove);

	/**
	 * Clears all action flags associated with the item.
	 * Designed to reset the item's action state for further configuration or reinitialization.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool ClearAllActionFlags();
	virtual bool ClearAllActionFlags_Implementation();
	
	UMounteaCallbackInventoryItemAction* GetCallbackItemAction() const
	{ return CallbackAction; };
	
public:
	
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const override;
};

#undef LOCTEXT_NAMESPACE