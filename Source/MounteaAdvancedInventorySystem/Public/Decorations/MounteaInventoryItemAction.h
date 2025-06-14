// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Definitions/MounteaInventoryItem.h"
#include "MounteaInventoryItemAction.generated.h"

class UTexture2D;
struct FInventoryItem;
class IMounteaAdvancedInventoryInterface;
class UGameplayEffect;

/**
 * Inventory item action implemented as a Gameplay Ability.
 * Leverages GAS for cooldowns, costs, targeting, and effect application.
 * 
 * This allows for powerful integration with existing ability systems while
 * providing inventory-specific functionality like visibility and permission checks.
 */
UCLASS(ClassGroup=(Mountea), Abstract, BlueprintType, Blueprintable, DisplayName="Inventory Item Action")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemAction : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UMounteaInventoryItemAction();

#pragma region Inventory Specific

	/**
	 * Display name of the action shown in the user interface.
	 * This is the text that players will see when interacting with items.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	FText ActionDisplayName;

	/**
	 * Brief description explaining what this action does.
	 * Used for tooltips and detailed action information in the UI.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	FText ActionDescription;

	/**
	 * Icon representing this action in the user interface.
	 * Used in action buttons, context menus, and other UI elements.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	TSoftObjectPtr<UTexture2D> ActionIcon;

	/**
	 * Priority value used for sorting actions in UI elements.
	 * Lower values appear first in lists and menus.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action", meta=(ClampMin=0))
	int32 ActionPriority = 0;

	/**
	 * Whether this action should be visible by default in the UI.
	 * Individual items can still override this through IsActionVisible checks.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	bool bIsVisibleByDefault = true;

	/**
	 * Whether this action requires confirmation before execution.
	 * Used for destructive or important actions like deletion or consumption.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	bool bRequiresConfirmation = false;

	/**
	 * Gameplay effects to apply when this action is successfully executed.
	 * These can modify attributes, apply temporary buffs, or trigger other game effects.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action")
	TArray<TSubclassOf<UGameplayEffect>> ActionEffects;

	#pragma endregion

	#pragma region Core Functions

public:

	/**
	 * Determines whether this action should be visible in the UI for the given item.
	 * This controls whether the action appears in context menus, action bars, etc.
	 *
	 * @param TargetItem The inventory item this action would be performed on
	 * @return True if the action should be visible to the user, false otherwise
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action")
	bool IsActionVisible(const FInventoryItem& TargetItem) const;

	/**
	 * Gets a user-friendly reason why the action is not allowed.
	 * Used for displaying helpful error messages or tooltips explaining restrictions.
	 *
	 * @param TargetItem The inventory item this action would be performed on
	 * @return Localized text explaining why the action cannot be executed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action")
	FText GetDisallowedReason(const FInventoryItem& TargetItem) const;

	/**
	 * Executes the inventory action using the GAS system.
	 * This is the main entry point for performing actions from the inventory UI.
	 *
	 * @param TargetItem The inventory item to perform the action on
	 * @return True if the action was successfully initiated
	 */
	UFUNCTION(BlueprintCallable, Category="Inventory Action")
	bool ExecuteInventoryAction(const FInventoryItem& TargetItem);

	#pragma endregion

	#pragma region Ability Overrides

protected:

	/**
	 * Override to add inventory-specific activation requirements.
	 * Checks item validity and visibility before allowing activation.
	 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;

	/**
	 * Main execution logic for the inventory action.
	 * Override this to implement specific action behavior.
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * Called when the ability ends, either from completion or cancellation.
	 * Useful for cleanup and state management.
	 */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	#pragma endregion

	#pragma region Blueprint Events

protected:

	/**
	 * Blueprint event called when the inventory action begins execution.
	 * Use this to implement custom action logic in Blueprint.
	 *
	 * @param TargetItem The inventory item being acted upon
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action", DisplayName="On Execute Action")
	void ReceiveExecuteAction(const FInventoryItem& TargetItem);

	/**
	 * Blueprint event called when the inventory action completes successfully.
	 * Use this for cleanup, notifications, or follow-up actions.
	 *
	 * @param TargetItem The inventory item that was acted upon
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action", DisplayName="On Action Completed")
	void ReceiveActionCompleted(const FInventoryItem& TargetItem);

	/**
	 * Blueprint event called when the inventory action fails or is cancelled.
	 *
	 * @param TargetItem The inventory item that was being acted upon
	 * @param Reason The reason for failure
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory Action", DisplayName="On Action Failed")
	void ReceiveActionFailed(const FInventoryItem& TargetItem, const FText& Reason);

	#pragma endregion

	#pragma region Helper Functions

protected:

	/**
	 * C++ implementation of visibility check.
	 * Override this for C++ visibility logic.
	 */
	virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const;

	/**
	 * C++ implementation of disallowed reason.
	 * Override this for C++ error message logic.
	 */
	virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const;

	/**
	 * Applies the configured gameplay effects to the ability owner.
	 * Called automatically after successful action execution.
	 */
	UFUNCTION(BlueprintCallable, Category="Inventory Action")
	void ApplyActionEffects();

	/**
	 * Gets the inventory item from the current ability execution context.
	 * Retrieves the item data passed during action execution.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory Action")
	FInventoryItem GetTargetItem() const;

	/**
	 * Gets the inventory interface from the ability owner.
	 * Used to interact with the owning inventory system.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory Action")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetOwningInventory() const;

	#pragma endregion

	#pragma region Private Data

private:

	/**
	 * The inventory item currently being processed by this action.
	 * Set during ability activation and cleared on completion.
	 */
	UPROPERTY(Transient)
	FInventoryItem CurrentTargetItem;

	#pragma endregion
};

/**
 * Utility class for creating inventory action gameplay event data.
 * Used to pass inventory item information through the GAS event system.
 */
UCLASS(BlueprintType)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UInventoryActionEventData : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * The inventory item associated with this event.
	 */
	UPROPERTY(BlueprintReadWrite, Category="Event Data")
	FInventoryItem TargetItem;

	/**
	 * Creates a new inventory action event data object.
	 */
	UFUNCTION(BlueprintCallable, Category="Inventory Action", CallInEditor)
	static UInventoryActionEventData* CreateEventData(const FInventoryItem& Item);
};