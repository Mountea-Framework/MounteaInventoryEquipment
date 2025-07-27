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
#include "Abilities/GameplayAbility.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Interfaces/ItemActions/MounteaAdvancedInventoryItemActionInterface.h"
#include "MounteaInventoryItemAction.generated.h"

class UTexture2D;
class UGameplayEffect;

/**
 * Inventory item action implemented as a Gameplay Ability.
 * Leverages GAS for cooldowns, costs, targeting, and effect application.
 */
UCLASS(ClassGroup=(Mountea), Abstract, BlueprintType, Blueprintable, DisplayName="Inventory Item Action")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemAction : public UGameplayAbility, public IMounteaAdvancedInventoryItemActionInterface
{
	GENERATED_BODY()

public:

	UMounteaInventoryItemAction();

#pragma region Inventory Specific

	/** Data of the Inventory Action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action",
		meta=(ShowOnlyInnerProperties))
	FMounteaItemActionData ItemActionData;

#pragma endregion

#pragma region IMounteaAdvancedInventoryItemActionInterface

	virtual FMounteaItemActionData GetActionData_Implementation() const override
	{ return ItemActionData; };
	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const override;
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const override;
	virtual bool ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem) override;
	virtual FGameplayTag GetInventoryItemTag_Implementation() const override
	{ return ItemActionData.ItemActionTag; }; 

#pragma endregion

#pragma region Ability Overrides

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

#pragma endregion

#pragma region Blueprint Events

	/**
	 * Blueprint event called when the inventory action begins execution.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action", DisplayName="On Execute Action")
	void ReceiveExecuteAction(const FInventoryItem& TargetItem);

	/**
	 * Blueprint event called when the inventory action completes successfully.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action", DisplayName="On Action Completed")
	void ReceiveActionCompleted(const FInventoryItem& TargetItem);

	/**
	 * Blueprint event called when the inventory action fails or is cancelled.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action", DisplayName="On Action Failed")
	void ReceiveActionFailed(const FInventoryItem& TargetItem, const FText& Reason);

#pragma endregion

#pragma region Helper Functions

	/**
	 * Main execution logic for the inventory action.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Inventory Action")
	bool ProcessAction(UObject* ActionInitiator, const FInventoryItem& TargetItem);
	virtual bool ProcessAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem);

	/**
	 * Applies the configured gameplay effects to the ability owner.
	 */
	void ApplyActionEffects();

	/**
	 * Gets the inventory item from the current ability execution context.
	 */
	FInventoryItem GetTargetItem() const;

	/**
	 * Gets the inventory interface from the ability owner.
	 */
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const;

#pragma endregion

#pragma region Private Data

private:
	
	/**
	 * The inventory item currently being processed by this action.
	 */
	UPROPERTY(Transient)
	FInventoryItem CurrentTargetItem;

#pragma endregion
};