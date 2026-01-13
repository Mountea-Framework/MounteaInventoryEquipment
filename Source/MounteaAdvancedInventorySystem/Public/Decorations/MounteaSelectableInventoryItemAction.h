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
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaSelectableInventoryItemAction : public UMounteaInventoryItemAction
{
	GENERATED_BODY()
	
public:
	
	UMounteaSelectableInventoryItemAction() :
		ActionDisplayName(LOCTEXT("DefaultActionName", "Default Action")),
		ActionDescription(LOCTEXT("DefaultActionDescription", "A basic inventory item action")),
		ActionPriority(0),
		bIsVisibleByDefault(true),
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
	 * Icon representing this action in the user interface.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	/**
	 * Priority value used for sorting actions in UI elements.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	int32 ActionPriority = INDEX_NONE;

	/**
	 * Whether this action should be visible by default in the UI.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		DisplayName="Available for UI")
	uint8 bIsVisibleByDefault : 1;

	/**
	 * The tag used to identify this action in the gameplay ability system.
	 * This tag is used for filtering and identifying actions in the UI and logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		meta=(Categories="Mountea_Inventory.ItemAction,Mountea_Inventory.ItemActions,ItemAction,Action"))
	FGameplayTagContainer ItemActionTag;

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
};

#undef LOCTEXT_NAMESPACE