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
#include "UObject/Object.h"

#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Definitions/MounteaInventoryItem.h"

#include "MounteaInventoryItemAction.generated.h"

/**
 * UMounteaInventoryItemAction provides lightweight inventory actions for local UI operations.
 * Simple actions execute directly without Gameplay Ability System overhead, making them ideal
 * for non-gameplay inventory operations like splitting stacks, moving items, or UI-only modifications
 * with immediate execution and minimal performance impact for inventory management tasks.
 *
 * @see [Simple Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/SimpleActions)
 * @see UMounteaInventoryItemAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea","Inventory Action","Mountea|Inventory Action"),
	HideCategories=("Cooking","Collision"),
	meta=(DisplayName="Mountea Inventory Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemAction : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * Initializes the action with the target item and owning inventory.
	 * This must be called before executing the action to establish the operating context.
	 * 
	 * @param TargetItem The inventory item this action will operate on.
	 * @param OwningInventory The inventory interface that contains the target item.
	 * @param ContextPayload Optional context object for additional data (e.g., player controller, UI context).
	 * 
	 * @return True if initialization was successful and action is ready to execute, false if setup failed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool InitializeItemAction(const FInventoryItem& TargetItem,
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& OwningInventory,
		UObject* ContextPayload = nullptr);
	virtual bool InitializeItemAction_Implementation(const FInventoryItem& TargetItem,
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& OwningInventory,
		UObject* ContextPayload = nullptr);

	/**
	 * Gets the inventory interface that owns the target item.
	 * 
	 * @return Script interface to the inventory containing the target item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory_Implementation() const;

	/**
	 * Validates that the target item can be modified by this action.
	 * 
	 * @return True if the item is in a valid state for modification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool CanModifyTargetItem() const;
	virtual bool CanModifyTargetItem_Implementation() const { return true; };
	
	/**
	 * Gets the action data containing display information and configuration.
	 * 
	 * @return The action data structure with name, description, icon, and other properties.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FMounteaItemActionData GetActionData() const;
	virtual FMounteaItemActionData GetActionData_Implementation() const { return ItemActionData;};

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
	 * Determines whether this action is currently allowed to be executed on the target item.
	 * 
	 * @param TargetItem The inventory item to check execution permissions for.
	 * 
	 * @return True if the action can be executed, false if it's currently blocked.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool IsAllowed(const FInventoryItem& TargetItem) const;
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const;

	/**
	 * Gets a user-friendly reason explaining why the action is not allowed.
	 * 
	 * @param TargetItem The inventory item that the action cannot be performed on.
	 * 
	 * @return Localized text explaining why the action is disabled or unavailable.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FText GetDisallowedReason(const FInventoryItem& TargetItem) const;
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const;

	/**
	 * Executes the inventory action on the specified target item.
	 * 
	 * This is the main entry point for all inventory actions, regardless of implementation type:
	 * - Simple actions: Execute logic directly and return result
	 * - GAS actions: Trigger GAS system which handles CanActivateAbility → ActivateAbility → EndAbility flow
	 * 
	 * @param TargetItem The inventory item to perform the action on.
	 * 
	 * @return True if the action was executed successfully, false if it failed.
	 * 
	 * @note For GAS-based actions, this function initiates the ability system workflow.
	 *	   The actual execution logic occurs in ActivateAbility() called by GAS.
	 * @note For simple actions, this function contains the complete execution logic.
	 * @note Always call InitializeItemAction() before executing actions to ensure proper context.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool ExecuteInventoryAction(const FInventoryItem& TargetItem);
	virtual bool ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem);

	/**
	 * Contains the core logic for this inventory action.
	 * 
	 * This is where the actual action work happens for both simple and GAS-based actions:
	 * - Simple actions: Called directly from ExecuteInventoryAction
	 * - GAS actions: Called from ActivateAbility after GAS validation and commitment
	 * 
	 * @param ActionInitiator The object that initiated this action (usually the actor performing it).
	 * @param TargetItem The inventory item to perform the action on.
	 * 
	 * @return True if the action logic executed successfully, false if it failed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool ProcessAction(UObject* ActionInitiator, const FInventoryItem& TargetItem);
	virtual bool ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem);

	/**
	 * Gets the gameplay tag that uniquely identifies this inventory action.
	 * 
	 * @return The gameplay tag used for filtering and identifying this action type.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FGameplayTag GetInventoryItemTag() const;
	virtual FGameplayTag GetInventoryItemTag_Implementation() const { return FGameplayTag(); };

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
	void AddActionFlag(const EInventoryItemActionCallback FlagToAdd);
	virtual void AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd);
	
	/**
	 * Removes a specific action flag from the item.
	 * Allows modification of action flags associated with inventory items.
	 *
	 * @param FlagToRemove The flag to be removed from the item's action flags.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void RemoveActionFlag(const EInventoryItemActionCallback FlagToRemove);
	virtual void RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove);

	/**
	 * Clears all action flags associated with the item.
	 * Designed to reset the item's action state for further configuration or reinitialization.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void ClearAllActionFlags();
	virtual void ClearAllActionFlags_Implementation();
	
protected:
	
	/** Data of the Inventory Action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action",
		meta=(ShowOnlyInnerProperties))
	FMounteaItemActionData ItemActionData;
};
