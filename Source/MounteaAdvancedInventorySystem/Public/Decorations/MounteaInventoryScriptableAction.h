// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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
#include "UObject/Object.h"

#include "MounteaInventoryScriptableAction.generated.h"

class UMounteaJsonObject;
/**
 * UMounteaInventoryScriptableAction is the common parent of Inventory Actions.
 *
 * @see UMounteaInventoryItemAction
 * @see UMounteaCraftingAction
 */
UCLASS(ClassGroup=(Mountea), Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew,
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Scriptable Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryScriptableAction : public UObject
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Initializes the action with contextual information.
	 * This must be called before executing the action to establish the operating context.
	 * 
	 * @param Payload JSON object which contains Action-specific data.
	 * 
	 * @return True if initialization was successful and action is ready to execute, false if setup failed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	bool InitializeInventoryAction(const UMounteaJsonObject* Payload);
	virtual bool InitializeInventoryAction_Implementation(const UMounteaJsonObject* Payload);
	
	/**
	 * Determines whether this action is currently allowed to be executed for the given payload.
	 * 
	 * @param Payload JSON object which contains Action-specific data.
	 * 
	 * @return True if the action can be executed, false if it's currently blocked.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	bool IsAllowed(const UMounteaJsonObject* Payload) const;
	virtual bool IsAllowed_Implementation(const UMounteaJsonObject* Payload) const;

	/**
	 * Gets a user-friendly reason explaining why the action is not allowed.
	 * 
	 * @param Payload JSON object which contains Action-specific data.
	 * 
	 * @return Localized text explaining why the action is disabled or unavailable.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	FText GetDisallowedReason(const UMounteaJsonObject* Payload) const;
	virtual FText GetDisallowedReason_Implementation(const UMounteaJsonObject* Payload) const;

	/**
	 * Executes the action for the given payload.
	 * 
	 * This is the main entry point for all actions, regardless of implementation type:
	 * - Selectable actions: execute logic and handle UI related work
	 * - Callback actions: Execute logic directly and returns result
	 * 
	 * @param Payload JSON object which contains Action-specific data.
	 * 
	 * @return True if the action was executed successfully, false if it failed.
	 * 
	 * @note Always call InitializeInventoryAction() before executing an action, so the action has its context.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	bool ExecuteInventoryAction(const UMounteaJsonObject* Payload);
	virtual bool ExecuteInventoryAction_Implementation(const UMounteaJsonObject* Payload);
	
	/**
	 * Cancels the action. Mostly used for delayed actions.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	void CancelInventoryAction();
	virtual void CancelInventoryAction_Implementation();

	/**
	 * Gets the gameplay tag that uniquely identifies this action.
	 * 
	 * @return The gameplay tag used for filtering and identifying this action type.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	FGameplayTagContainer GetInventoryActionTags() const;
	virtual FGameplayTagContainer GetInventoryActionTags_Implementation() const
	{
		return ItemActionTags;
	};
	
	/**
	 * Called once the action has finished or been cancelled, to release its resources.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Inventory Actions")
	void CleanupInventoryAction();
	virtual void CleanupInventoryAction_Implementation() {};
	
protected:
	
	/**
	 * The tag used to identify this action type.
	 * This tag is used for filtering and identifying actions in the UI and logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		meta=(Categories="Mountea_Inventory.ItemAction,Mountea_Inventory.ItemActions,ItemAction,Action"))
	FGameplayTagContainer ItemActionTags;
	
protected:
	
	/**
	 * ContextPayload is a transient JSON object associated with the inventory action.
	 * It holds contextual information required for initializing or executing the action
	 * and is typically set during the initialization phase of the calling operation.
	 */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Configuration")
	TObjectPtr<UMounteaJsonObject> ContextPayload;
};
