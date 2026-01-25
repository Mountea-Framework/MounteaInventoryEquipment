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
 * @see [Simple Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemActions)
 * @see UMounteaInventoryItemAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, NotBlueprintable, BlueprintType, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea","Inventory Action","Mountea|Inventory Action"),
	HideCategories=("Cooking","Collision","Private"),
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
	bool InitializeItemAction(const FInventoryItem& TargetItem, const TScriptInterface<IMounteaAdvancedInventoryInterface>& OwningInventory,
		UObject* ContextPayload = nullptr);
	virtual bool InitializeItemAction_Implementation(const FInventoryItem& TargetItem, const TScriptInterface<IMounteaAdvancedInventoryInterface>& OwningInventory,
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
	 * - Selectable actions: Execute logic and does UI related stuff
	 * - Callback actions: Execute logic directly and returns result
	 * 
	 * @param TargetItem The inventory item to perform the action on.
	 * 
	 * @return True if the action was executed successfully, false if it failed.
	 * 
	 * @note Always call InitializeItemAction() before executing actions to ensure proper context.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool ExecuteInventoryAction(const FInventoryItem& TargetItem);
	virtual bool ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem);

	/**
	 * Cancels the Inventory Action. This should be called mostly from delayed Actions!
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void CancelInventoryAction();
	virtual void CancelInventoryAction_Implementation();

	/**
	 * Gets the gameplay tag that uniquely identifies this inventory action.
	 * 
	 * @return The gameplay tag used for filtering and identifying this action type.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FGameplayTagContainer GetItemActionTags() const;
	virtual FGameplayTagContainer GetItemActionTags_Implementation() const { return ItemActionTags; };

	/**
	 * Gets called once Action has finished/cancelled to restore resources.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void CleanupInventoryAction();
	virtual void CleanupInventoryAction_Implementation() {};

protected:
	
	/**
	 * The tag used to identify this action in the gameplay ability system.
	 * This tag is used for filtering and identifying actions in the UI and logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		meta=(Categories="Mountea_Inventory.ItemAction,Mountea_Inventory.ItemActions,ItemAction,Action"))
	FGameplayTagContainer ItemActionTags;
};
