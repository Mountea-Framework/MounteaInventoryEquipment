// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaInventoryBaseDataTypes.generated.h"

class UMounteaInventoryItemAction;
class UTexture;

#define LOCTEXT_NAMESPACE "InventoryRarity"

/**
 * Structure defining inventory rarity properties.
 */
USTRUCT(BlueprintType)
struct FInventoryRarity
{
	GENERATED_BODY()

	FInventoryRarity();

	/**
	 * The name of the item rarity (e.g., Common, Rare, Epic).
	 */
	UPROPERTY(EditAnywhere)
	FText RarityDisplayName = LOCTEXT("InventoryRarity_DisplayName", "");

	/**
	 * The visual color associated with the rarity for UI purposes.
	 */
	UPROPERTY(EditAnywhere)
	FLinearColor RarityColor;

	/**
	 * A multiplier applied to item trade or sale prices for this rarity.
	 */
	UPROPERTY(EditAnywhere)
	float BasePriceMultiplier = 1.f;

	/**
	 * Tags that define additional metadata or rules for this rarity.
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer RarityTags;
};

#undef LOCTEXT_NAMESPACE

#define LOCTEXT_NAMESPACE "InventoryCategory"

/**
 * Structure defining inventory category properties.
 */
USTRUCT(BlueprintType)
struct FInventoryCategory
{
	GENERATED_BODY()

	FInventoryCategory();

	/**
	 * The name of the item category (e.g., Weapon, Armor, Consumable), localized.
	 */
	UPROPERTY(EditAnywhere)
	FText CategoryDisplayName = LOCTEXT("InventoryCategory_DisplayName", "");

	/**
	 * Priority for sorting categories in UI or other contexts.
	 */
	UPROPERTY(EditAnywhere)
	int32 CategoryPriority = 0;

	/**
	 * Tags that define this Category.
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer CategoryTags;

	/**
	 * Icon used in the UI to represent this category.
	 */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture> DisplayIcon;
	
	/**
	 * Optional bitmask for category-specific behaviors (e.g., Craftable, Equipable).
	 */
	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags"))
	uint8 CategoryFlags;

	/**
	 * Defines a list of allowed Actions.
	 * Each Item of this category can perform those Actions.
	 */
	UPROPERTY(EditAnywhere)
	TSet<TSoftClassPtr<UMounteaInventoryItemAction>> AllowedActions;
};

#undef LOCTEXT_NAMESPACE

// Equality operator for FInventoryRarity
FORCEINLINE bool operator==(const FInventoryRarity& LHS, const FInventoryRarity& RHS)
{
	return LHS.RarityDisplayName.EqualTo(RHS.RarityDisplayName) &&
		   LHS.RarityColor == RHS.RarityColor &&
		   FMath::IsNearlyEqual(LHS.BasePriceMultiplier, RHS.BasePriceMultiplier) &&
		   LHS.RarityTags == RHS.RarityTags;
}

// Inequality operator for FInventoryRarity
FORCEINLINE bool operator!=(const FInventoryRarity& LHS, const FInventoryRarity& RHS)
{
	return !(LHS == RHS);
}

// Equality operator for FInventoryCategory
FORCEINLINE bool operator==(const FInventoryCategory& LHS, const FInventoryCategory& RHS)
{
	return LHS.CategoryDisplayName.EqualTo(RHS.CategoryDisplayName) &&
		   LHS.CategoryPriority == RHS.CategoryPriority &&
		   LHS.CategoryTags == RHS.CategoryTags &&
		   LHS.DisplayIcon == RHS.DisplayIcon &&
		   LHS.CategoryFlags == RHS.CategoryFlags;
}

// Inequality operator for FInventoryCategory
FORCEINLINE bool operator!=(const FInventoryCategory& LHS, const FInventoryCategory& RHS)
{
	return !(LHS == RHS);
}