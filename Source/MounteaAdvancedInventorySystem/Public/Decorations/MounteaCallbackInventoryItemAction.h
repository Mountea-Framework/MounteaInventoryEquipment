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
#include "MounteaCallbackInventoryItemAction.generated.h"

/**
 * UMounteaCallbackInventoryItemAction provides the "Callable" counterpart to Selectable UI actions.
 * This class is responsible for executing the actual gameplay / system logic triggered from the UI layer.
 *
 * Callback Actions are intended to be invoked by "UMounteaSelectableInventoryItemAction" after the UI
 * finishes its local-only interaction flow (for example selecting a target, choosing a quantity to consume,
 * confirming an action in a modal window, etc.).
 *
 * Unlike Selectable actions, Callback Actions are allowed to perform cross-system operations and can
 * interact with gameplay subsystems such as Inventory, Equipment, GAS, quests, or any other runtime logic.
 *
 * Additionally, this action may apply configured Gameplay Effects when execution succeeds.
 * The "ActionEffects" array specifies GameplayEffect classes to be applied as part of the successful
 * execution flow (typically through GAS), allowing actions like Consume/Equip/Use to grant buffs, debuffs,
 * cooldowns, or any other effect-driven consequences.
 *
 * @see [Simple Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/SimpleActions)
 * @see UMounteaSelectableInventoryItemAction
 * @see UMounteaInventoryItemAction
 */

UCLASS(ClassGroup=(Mountea), Abstract, Blueprintable, meta=(DisplayName="Callback Item Action"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCallbackInventoryItemAction : public UMounteaInventoryItemAction
{
	GENERATED_BODY()
	
protected:
	
	/**
	 * Gameplay effects to apply when this action is successfully executed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		DisplayName="Action Effects (GAS)")
	TArray<TSoftClassPtr<UGameplayEffect>> ActionEffects;
};
