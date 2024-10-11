// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaInventoryItemRarity.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemRarity"

/**
 * Mountea Inventory Item Rarity.
 *
 * Represents a rarity within the Mountea Inventory & Equipment system.
 * Rarities are used to categorize and sort inventory items based on their rarity level.
 * Each rarity includes properties like a unique identifier, name, description, visual representation, tag, color, and price coefficient.
 *
 * This class allows for flexible definition of item rarities, enabling features like visual differentiation, pricing adjustments, and rarity-based behaviors.
 *
 * @see UDataAsset
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemRarity : public UDataAsset
{
    GENERATED_BODY()

protected:

    /** Unique identifier for the rarity. Used to distinguish this rarity from others. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Required")
    FGuid RarityGuid;

    /** Display name of the rarity. Used for UI representation and identification. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required")
    FText RarityName = LOCTEXT("MounteaInventoryItemRarity_Name", "Default");
    
    /** Gameplay tag associated with the rarity. Can be used for filtering, querying, or applying specific behaviors based on the tag. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required")
    FGameplayTag RarityTag;
    
    /** Visual representation of the rarity. Used to display an icon or image associated with the rarity in the UI. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required")
    FSlateBrush RarityBrush;
    
    /** Color associated with the rarity. Used for UI elements to represent the rarity's color scheme. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(HideAlphaChannel))
    FLinearColor RarityColor = FLinearColor::Gray;

    /** Description of the rarity. Provides additional information about the rarity. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="2. Optional")
    FText RarityDescription = LOCTEXT("MounteaInventoryItemRarity_Description", "");

    /** Price coefficient for the rarity. Used to adjust item prices based on rarity. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="2. Optional")
    float RarityPriceCoefficient = 1.f;

public:

    /**
     * Gets the unique identifier of the rarity.
     *
     * @return The FGuid representing the rarity's unique ID.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE FGuid GetRarityGuid() const { return RarityGuid; }

    /**
     * Sets the unique identifier of the rarity.
     *
     * @param newGuid The new FGuid to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityGuid(const FGuid& newGuid) { RarityGuid = newGuid; }

    /**
     * Gets the display name of the rarity.
     *
     * @return The FText representing the rarity's name.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE FText GetRarityName() const { return RarityName; }

    /**
     * Sets the display name of the rarity.
     *
     * @param newName The new name to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityName(const FText& newName) { RarityName = newName; }

    /**
     * Gets the gameplay tag associated with the rarity.
     *
     * @return The FGameplayTag representing the rarity's tag.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE FGameplayTag GetRarityTag() const { return RarityTag; }

    /**
     * Sets the gameplay tag associated with the rarity.
     *
     * @param newTag The new FGameplayTag to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityTag(const FGameplayTag& newTag) { RarityTag = newTag; }

    /**
     * Gets the visual representation of the rarity.
     *
     * @return The FSlateBrush representing the rarity's icon or image.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE FSlateBrush GetRarityBrush() const { return RarityBrush; }

    /**
     * Sets the visual representation of the rarity.
     *
     * @param newBrush The new FSlateBrush to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityBrush(const FSlateBrush& newBrush) { RarityBrush = newBrush; }

    /**
     * Gets the color associated with the rarity.
     *
     * @return The FLinearColor representing the rarity's color.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE FLinearColor GetRarityColor() const { return RarityColor; }

    /**
     * Sets the color associated with the rarity.
     *
     * @param newColor The new FLinearColor to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityColor(const FLinearColor& newColor) { RarityColor = newColor; }

    /**
     * Gets the description of the rarity.
     *
     * @return The FText representing the rarity's description.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE FText GetRarityDescription() const { return RarityDescription; }

    /**
     * Sets the description of the rarity.
     *
     * @param newDescription The new description to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityDescription(const FText& newDescription) { RarityDescription = newDescription; }

    /**
     * Gets the price coefficient of the rarity.
     *
     * @return The float representing the rarity's price coefficient.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Getter"))
    FORCEINLINE float GetRarityPriceCoefficient() const { return RarityPriceCoefficient; }

    /**
     * Sets the price coefficient of the rarity.
     *
     * @param newCoefficient The new price coefficient to assign to the rarity.
     */
    UFUNCTION(BlueprintCallable, Category="Mountea|Inventory&Equipment|Rarity", meta=(CustomTag="MounteaK2Setter"))
    FORCEINLINE void SetRarityPriceCoefficient(float newCoefficient) { RarityPriceCoefficient = newCoefficient; }
};

#undef LOCTEXT_NAMESPACE