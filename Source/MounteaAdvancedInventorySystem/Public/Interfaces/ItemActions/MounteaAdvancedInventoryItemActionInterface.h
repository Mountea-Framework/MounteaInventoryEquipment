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
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryItemActionInterface.generated.h"

class UGameplayEffect;
class IMounteaAdvancedInventoryInterface;
struct FInventoryItem;

enum class EInventoryItemActionCallback : uint8;

#define LOCTEXT_NAMESPACE "MounteaInventoryItemActionData"

/**
 * Data structure for defining item action properties.
 * Used to configure actions in the editor and at runtime.
 */
USTRUCT(BlueprintType)
struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaItemActionData
{
	GENERATED_BODY()

	FMounteaItemActionData() : 
		ActionDisplayName(LOCTEXT("DefaultActionName", "Default Action")),
		ActionDescription(LOCTEXT("DefaultActionDescription", "A basic inventory item action")),
		ActionPriority(0),
		bIsVisibleByDefault(true),
		bRequiresConfirmation(false),
		InventoryItemActionCallback(0)
	{}

public:
	
	/**
	 * Display name of the action shown in the user interface.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	FText ActionDisplayName;

	/**
	 * Brief description explaining what this action does.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	FText ActionDescription;

	/**
	 * Icon representing this action in the user interface.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	TSoftObjectPtr<UTexture2D> Icon;

	/**
	 * Priority value used for sorting actions in UI elements.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	int32 ActionPriority = 0;

	/**
	 * Whether this action should be visible by default in the UI.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	uint8 bIsVisibleByDefault : 1;

	/**
	 * Whether this action requires confirmation before execution.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	uint8 bRequiresConfirmation : 1;

	/**
	 * The tag used to identify this action in the gameplay ability system.
	 * This tag is used for filtering and identifying actions in the UI and logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	FGameplayTag ItemActionTag;

	/**
	 * Callback type for this action, used to determine how the action is processed.
	 * This can be used to trigger different behaviors based on the action type.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action",
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemActionCallback"))
	uint8 InventoryItemActionCallback;

	/**
	 * Gameplay effects to apply when this action is successfully executed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action",
		DisplayName="(Optional) Action Effects")
	TArray<TSoftClassPtr<UGameplayEffect>> ActionEffects;
};

#undef LOCTEXT_NAMESPACE

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemActionInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemActionInterface
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
		UObject* ContextPayload = nullptr) = 0;

	/**
	 * Checks if this action has been properly initialized and is ready for execution.
	 * 
	 * @return True if Initialize() was called successfully, false if action needs setup.
	 */
	/*
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool IsItemActionInitialized() const;
	virtual bool IsItemActionInitialized_Implementation() const = 0;
	*/

	/**
	 * Resets the action to uninitialized state, clearing target item and inventory references.
	 */
	/*
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void ResetItemAction();
	virtual void ResetItemAction_Implementation() = 0;
	*/

	/**
	 * Gets the inventory item currently being processed by this action.
	 * 
	 * @return The target item this action is operating on.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FInventoryItem GetTargetItem() const;
	virtual FInventoryItem GetTargetItem_Implementation() const = 0;

	/**
	 * Sets the target item for this action to operate on.
	 * 
	 * @param TargetItem The inventory item to set as the target.
	 */
	/*
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void SetTargetItem(const FInventoryItem& TargetItem);
	virtual void SetTargetItem_Implementation(const FInventoryItem& TargetItem) = 0;
	*/

	/**
	 * Gets the inventory interface that owns the target item.
	 * 
	 * @return Script interface to the inventory containing the target item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory_Implementation() const = 0;

	/**
	 * Validates that the target item can be modified by this action.
	 * 
	 * @return True if the item is in a valid state for modification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool CanModifyTargetItem() const;
	virtual bool CanModifyTargetItem_Implementation() const = 0;
	
	/**
	 * Gets the action data containing display information and configuration.
	 * 
	 * @return The action data structure with name, description, icon, and other properties.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FMounteaItemActionData GetActionData() const;
	virtual FMounteaItemActionData GetActionData_Implementation() const = 0;

	/**
	 * Determines whether this action should be visible in the UI for the given item.
	 * 
	 * @param TargetItem The inventory item to check visibility for.
	 * 
	 * @return True if the action should be shown in the user interface, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool IsActionVisible(const FInventoryItem& TargetItem) const;
	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const = 0;

	/**
	 * Determines whether this action is currently allowed to be executed on the target item.
	 * 
	 * @param TargetItem The inventory item to check execution permissions for.
	 * 
	 * @return True if the action can be executed, false if it's currently blocked.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	bool IsAllowed(const FInventoryItem& TargetItem) const;
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const = 0;

	/**
	 * Gets a user-friendly reason explaining why the action is not allowed.
	 * 
	 * @param TargetItem The inventory item that the action cannot be performed on.
	 * 
	 * @return Localized text explaining why the action is disabled or unavailable.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FText GetDisallowedReason(const FInventoryItem& TargetItem) const;
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const = 0;

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
	virtual bool ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem) = 0;

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
	virtual bool ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem) = 0;

	/**
	 * Gets the gameplay tag that uniquely identifies this inventory action.
	 * 
	 * @return The gameplay tag used for filtering and identifying this action type.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	FGameplayTag GetInventoryItemTag() const;
	virtual FGameplayTag GetInventoryItemTag_Implementation() const = 0;

	/**
	 * Gets the callback type for this inventory item action.
	 * 
	 * This defines how the action interacts with the inventory system and UI.
	 * 
	 * @return The callback type indicating how this action should be processed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	EInventoryItemActionCallback GetInventoryItemActionCallback() const;
	virtual EInventoryItemActionCallback GetInventoryItemActionCallback_Implementation() const = 0;

	/**
	 * Adds a specified action flag to an inventory item action.
	 * Designed to modify the behavior of item actions by appending a predefined flag.
	 *
	 * @param FlagToAdd The action flag to be added to the item action.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void AddActionFlag(const EInventoryItemActionCallback FlagToAdd);
	virtual void AddActionFlag_Implementation(const EInventoryItemActionCallback FlagToAdd) = 0;
	
	/**
	 * Removes a specific action flag from the item.
	 * Allows modification of action flags associated with inventory items.
	 *
	 * @param FlagToRemove The flag to be removed from the item's action flags.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void RemoveActionFlag(const EInventoryItemActionCallback FlagToRemove);
	virtual void RemoveActionFlag_Implementation(const EInventoryItemActionCallback FlagToRemove) = 0;

	/**
	 * Clears all action flags associated with the item.
	 * Designed to reset the item's action state for further configuration or reinitialization.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Item Actions")
	void ClearAllActionFlags();
	virtual void ClearAllActionFlags_Implementation() = 0;
};