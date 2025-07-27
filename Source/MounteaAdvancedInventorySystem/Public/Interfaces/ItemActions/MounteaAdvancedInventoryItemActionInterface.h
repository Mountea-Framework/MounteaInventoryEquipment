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

struct FInventoryItem;

/**
 * Data structure for defining item action properties.
 * Used to configure actions in the editor and at runtime.
 */
USTRUCT(BlueprintType)
struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaItemActionData
{
	GENERATED_BODY()

	/**
	 * Display name of the action shown in the user interface.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	FText ActionDisplayName;

	/**
	 * Brief description explaining what this action does.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	FText ActionDescription;

	/**
	 * Icon representing this action in the user interface.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	TSoftObjectPtr<UTexture2D> Icon;

	/**
	 * Priority value used for sorting actions in UI elements.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	int32 ActionPriority = 0;

	/**
	 * Whether this action should be visible by default in the UI.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	bool bIsVisibleByDefault = true;

	/**
	 * Whether this action requires confirmation before execution.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	bool bRequiresConfirmation = false;

	/**
	 * The tag used to identify this action in the gameplay ability system.
	 * This tag is used for filtering and identifying actions in the UI and logic.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Inventory Action")
	FGameplayTag ItemActionTag;

	/**
	 * Gameplay effects to apply when this action is successfully executed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory Action",
		DisplayName="(Optional) Action Effects")
	TArray<TSoftClassPtr<UGameplayEffect>> ActionEffects;

	FMounteaItemActionData() = default;
};

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemActionInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemActionInterface
{
	GENERATED_BODY()

public:
	
	public:
    /**
     * Gets the action data containing display information and configuration.
     * 
     * @return The action data structure with name, description, icon, and other properties.
     */
    UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions")
    FMounteaItemActionData GetActionData() const;
    virtual FMounteaItemActionData GetActionData_Implementation() const = 0;

    /**
     * Determines whether this action should be visible in the UI for the given item.
     * 
     * @param TargetItem The inventory item to check visibility for.
     * 
     * @return True if the action should be shown in the user interface, false otherwise.
     */
    UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions")
    bool IsActionVisible(const FInventoryItem& TargetItem) const;
    virtual bool IsActionVisible_Implementation(const FInventoryItem& TargetItem) const = 0;

    /**
     * Determines whether this action is currently allowed to be executed on the target item.
     * 
     * @param TargetItem The inventory item to check execution permissions for.
     * 
     * @return True if the action can be executed, false if it's currently blocked.
     */
    UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions")
    bool IsAllowed(const FInventoryItem& TargetItem) const;
    virtual bool IsAllowed_Implementation(const FInventoryItem& TargetItem) const = 0;

    /**
     * Gets a user-friendly reason explaining why the action is not allowed.
     * 
     * @param TargetItem The inventory item that the action cannot be performed on.
     * 
     * @return Localized text explaining why the action is disabled or unavailable.
     */
    UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions")
    FText GetDisallowedReason(const FInventoryItem& TargetItem) const;
    virtual FText GetDisallowedReason_Implementation(const FInventoryItem& TargetItem) const = 0;

    /**
     * Executes the inventory action on the specified target item.
     * 
     * @param TargetItem The inventory item to perform the action on.
     * 
     * @return True if the action was executed successfully, false if it failed.
     */
    UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions")
    bool ExecuteInventoryAction(const FInventoryItem& TargetItem);
    virtual bool ExecuteInventoryAction_Implementation(const FInventoryItem& TargetItem) = 0;

    /**
     * Gets the gameplay tag that uniquely identifies this inventory action.
     * 
     * @return The gameplay tag used for filtering and identifying this action type.
     */
    UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions")
    FGameplayTag GetInventoryItemTag() const;
    virtual FGameplayTag GetInventoryItemTag_Implementation() const = 0;
};
