// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryItemBlueprint.generated.h"

class UMounteaInventoryItemAction;

/**
 * Mountea Inventory Item Blueprint.
 *
 * Represents the blueprint for an inventory item within the Mountea Inventory system.
 * This class serves as a data asset that holds default properties and actions for items.
 * It includes identifiers, tags, primary and optional data, and associated item actions.
 *
 * @see UDataAsset
 * @see UMounteaInventoryItemAction
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea", DisplayName="Inventory Item Blueprint")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBlueprint : public UDataAsset
{
	GENERATED_BODY()

public:

	UMounteaInventoryItemBlueprint();

protected:

	/** Unique identifier for the item blueprint. Used to distinguish this blueprint from others. */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Required")
	FGuid BlueprintGuid = FGuid();;

	/** Default gameplay tags associated with items created from this blueprint. */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Required")
	FGameplayTagContainer DefaultTags;

	/** Primary data required for items created from this blueprint. */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Required")
	FMounteaInventoryItemRequiredData PrimaryData;

	/** Optional secondary data for items created from this blueprint. */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Optional")
	FMounteaInventoryItemOptionalData SecondaryData;

	/** Set of actions associated with items created from this blueprint. */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Optional")
	TSet<TObjectPtr<UMounteaInventoryItemAction>> ItemActions;

public:

	/**
	 * Gets the unique identifier of the blueprint.
	 *
	 * @return The FGuid representing the blueprint's unique ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprint", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGuid GetBlueprintGuid() const { return BlueprintGuid; }

	/**
	 * Sets the unique identifier of the blueprint.
	 *
	 * @param NewGuid The new FGuid to assign to the blueprint.
	 */
	FORCEINLINE void SetBlueprintGuid(const FGuid& NewGuid) { BlueprintGuid = NewGuid; }

	/**
	 * Gets the default gameplay tags associated with the blueprint.
	 *
	 * @return The FGameplayTagContainer containing the default tags.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprint", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGameplayTagContainer GetDefaultTags() const { return DefaultTags; }

	/**
	 * Sets the default gameplay tags associated with the blueprint.
	 *
	 * @param NewTags The new FGameplayTagContainer to assign to the blueprint.
	 */
	FORCEINLINE void SetDefaultTags(const FGameplayTagContainer& NewTags) { DefaultTags = NewTags; }

	/**
	 * Gets the primary data required for items created from this blueprint.
	 *
	 * @return The FMounteaInventoryItemRequiredData structure.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprint", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FMounteaInventoryItemRequiredData GetPrimaryData() const { return PrimaryData; }

	/**
	 * Sets the primary data required for items created from this blueprint.
	 *
	 * @param NewPrimaryData The new FMounteaInventoryItemRequiredData to assign.
	 */
	FORCEINLINE void SetPrimaryData(const FMounteaInventoryItemRequiredData& NewPrimaryData) { PrimaryData = NewPrimaryData; }

	/**
	 * Gets the optional secondary data for items created from this blueprint.
	 *
	 * @return The FMounteaInventoryItemOptionalData structure.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprint", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FMounteaInventoryItemOptionalData GetSecondaryData() const { return SecondaryData; }

	/**
	 * Sets the optional secondary data for items created from this blueprint.
	 *
	 * @param NewSecondaryData The new FMounteaInventoryItemOptionalData to assign.
	 */
	FORCEINLINE void SetSecondaryData(const FMounteaInventoryItemOptionalData& NewSecondaryData) { SecondaryData = NewSecondaryData; }

	/**
	 * Gets the actions associated with items created from this blueprint.
	 *
	 * @return An array of pointers to the item actions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprint", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE TArray<UMounteaInventoryItemAction*> GetItemActions() const { return ItemActions.Array(); }

	/**
	 * Sets the actions associated with items created from this blueprint.
	 *
	 * @param NewActions The new array of actions to assign.
	 */
	FORCEINLINE void SetItemActions(const TArray<UMounteaInventoryItemAction*>& NewActions)
	{
		for (const auto& newAction : NewActions)
		{
			if (newAction && !ItemActions.Contains(newAction))
				ItemActions.Add(newAction);
		}
	}
};

