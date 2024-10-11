// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaItemAction.h"
#include "Helpers/MounteaInventoryFlagsLibrary.h"
#include "MounteaInventoryItemCategory.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemCategory"

/**
 * Mountea Inventory Item Category.
 *
 * Represents a category within the Mountea Inventory & Equipment system.
 * Categories are used to sort and organize inventory items.
 * Each category includes properties like a unique identifier, name, description, visual representation, tags, flags, and associated actions.
 *
 * This class allows for flexible categorization of items, enabling features like filtering, grouping, and specific behaviors tied to categories.
 *
 * @see UDataAsset
 * @see UMounteaInventoryItemAction
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemCategory : public UDataAsset
{
	GENERATED_BODY()

protected:

	/** Unique identifier for the category. Used to distinguish this category from others. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required")
	FGuid CategoryGuid;

	/** Display name of the category. Used for UI representation and identification. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required")
	FText CategoryName = LOCTEXT("MounteaInventoryItemCategory_Name", "Category Name");

	/** Description of the category. Provides additional information about the category. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required", meta=(MultiLine))
	FText CategoryDescription = LOCTEXT("MounteaInventoryItemCategory_Description", "Default");

	/** Visual representation of the category. Used to display an icon or image associated with the category in the UI. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required")
	FSlateBrush CategoryBrush;
	
	/** Gameplay tags associated with the category. Can be used for filtering, querying, or applying specific behaviors based on tags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required")
	FGameplayTagContainer CategoryTags;

	/** Flags defining special properties or behaviors of the category. Uses the EInventoryCategoryFlags enum as a bitmask. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required", meta=(Bitmask, BitmaskEnum="EInventoryCategoryFlags"))
	uint8 CategoryFlags;
	
	/** Set of actions associated with the category. Actions can define custom behaviors or interactions for items in this category. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="2. Optional")
	TSet<TObjectPtr<UMounteaInventoryItemAction>> CategoryActions;

public:

	/**
	 * Gets the unique identifier of the category.
	 *
	 * @return The FGuid representing the category's unique ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGuid GetCategoryGuid() const { return CategoryGuid; }

	/**
	 * Sets the unique identifier of the category.
	 *
	 * @param newGuid The new FGuid to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetCategoryGuid(const FGuid& newGuid) { CategoryGuid = newGuid; }

	/**
	 * Gets the display name of the category.
	 *
	 * @return The FText representing the category's name.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FText GetCategoryName() const { return CategoryName; }

	/**
	 * Sets the display name of the category.
	 *
	 * @param newName The new name to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetCategoryName(const FText& newName) { CategoryName = newName; }

	/**
	 * Gets the description of the category.
	 *
	 * @return The FText representing the category's description.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FText GetCategoryDescription() const { return CategoryDescription; }

	/**
	 * Sets the description of the category.
	 *
	 * @param newDescription The new description to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetCategoryDescription(const FText& newDescription) { CategoryDescription = newDescription; }

	/**
	 * Gets the visual representation of the category.
	 *
	 * @return The FSlateBrush representing the category's icon or image.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FSlateBrush GetCategoryBrush() const { return CategoryBrush; }

	/**
	 * Sets the visual representation of the category.
	 *
	 * @param newBrush The new FSlateBrush to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetCategoryBrush(const FSlateBrush& newBrush) { CategoryBrush = newBrush; }

	/**
	 * Gets the gameplay tags associated with the category.
	 *
	 * @return The FGameplayTagContainer containing the category's tags.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE FGameplayTagContainer GetCategoryTags() const { return CategoryTags; }

	/**
	 * Sets the gameplay tags associated with the category.
	 *
	 * @param newTags The new FGameplayTagContainer to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetCategoryTags(const FGameplayTagContainer& newTags) { CategoryTags = newTags; }

	/**
	 * Gets the flags defining special properties or behaviors of the category.
	 *
	 * @return The EInventoryCategoryFlags representing the category's flags.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter", Bitmask, BitmaskEnum="EInventoryCategoryFlags"))
	FORCEINLINE EInventoryItemFlags GetCategoryFlags() const { return static_cast<EInventoryItemFlags>(CategoryFlags); }

	/**
	 * Sets the flags defining special properties or behaviors of the category.
	 *
	 * @param newFlags The new EInventoryCategoryFlags to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter", Bitmask, BitmaskEnum="EInventoryCategoryFlags"))
	FORCEINLINE void SetCategoryFlags(EInventoryItemFlags newFlags) { CategoryFlags = static_cast<uint8>(newFlags); }

	/**
	 * Gets the actions associated with the category.
	 *
	 * @return An array of pointers to the category's actions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Getter"))
	FORCEINLINE TArray<UMounteaInventoryItemAction*> GetCategoryActions() const { return CategoryActions.Array(); }

	/**
	 * Sets the actions associated with the category.
	 *
	 * @param newActions The new array of actions to assign to the category.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Category", meta=(CustomTag="MounteaK2Setter"))
	FORCEINLINE void SetCategoryActions(const TArray<UMounteaInventoryItemAction*>& newActions)
	{
		for (const auto& newAction : newActions)
		{
			if (newAction && !CategoryActions.Contains(newAction))
				CategoryActions.Add(newAction);
		}
	}
};

#undef LOCTEXT_NAMESPACE