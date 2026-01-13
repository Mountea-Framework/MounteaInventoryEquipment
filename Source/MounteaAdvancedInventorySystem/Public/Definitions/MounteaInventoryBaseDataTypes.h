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
#include "MounteaInventoryBaseEnums.h"
#include "MounteaInventoryBaseDataTypes.generated.h"

class UMounteaInventoryItemAction;
class UTexture;
class UTexture2D;
class UGameplayEffect;

enum class EInventoryItemActionCallback : uint8;

#pragma region InventoryRarity
#define LOCTEXT_NAMESPACE "InventoryRarity"

/**
 * FInventoryRarity defines the visual and economic properties of item rarity levels.
 * Rarity structures control item appearance through color coding, economic value through price multipliers,
 * and gameplay behavior through associated tags for consistent item classification systems.
 *
 * @see [Item Rarity System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemRarity)
 * @see UMounteaInventoryItemTemplate
 */
USTRUCT(BlueprintType)
struct FInventoryRarity
{
	GENERATED_BODY()

	FInventoryRarity();

	/**
	 * The name of the item rarity (e.g., Common, Rare, Epic).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Rarity")
	FText RarityDisplayName = LOCTEXT("InventoryRarity_DisplayName", "");

	/**
	 * The visual color associated with the rarity for UI purposes.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Rarity")
	FLinearColor RarityColor;
	
	// Rarity priority used for Sorting.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Rarity")
	int32 RarityPriority = 0;

	/**
	 * A multiplier applied to item trade or sale prices for this rarity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Rarity")
	float BasePriceMultiplier = 1.f;

	/**
	 * Tags that define additional metadata or rules for this rarity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Rarity")
	FGameplayTagContainer RarityTags;
};

#undef LOCTEXT_NAMESPACE
#pragma endregion

#pragma region InventoryCategory
#define LOCTEXT_NAMESPACE "InventoryCategory"

/**
 * FInventoryCategoryData contains the configuration and metadata for inventory item categories.
 * Category data defines display properties, sorting priorities, associated tags, visual icons,
 * behavioral flags, and allowed actions for comprehensive item classification.
 *
 * @see [Item Categories](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemCategories)
 * @see FInventoryCategory
 * @see UMounteaInventoryItemAction
 */
USTRUCT(BlueprintType)
struct FInventoryCategoryData
{
	GENERATED_BODY()

	FInventoryCategoryData();

	/**
	 * The name of the item category (e.g., Weapon, Armor, Consumable), localized.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Category")
	FText CategoryDisplayName = LOCTEXT("InventoryCategory_DisplayName", "");

	/**
	 * Priority for sorting categories in UI or other contexts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Category")
	int32 CategoryPriority = 0;

	/**
	 * Tags that define this Category.
	 * By default filtered for:
	 * → Mountea_Inventory.Categories
	 * → Mountea_Inventory.Category
	 * → Categories
	 * → Category
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Category",
		meta=(Categories="Mountea_Inventory.Categories,Mountea_Inventory.Category,Categories,Category"))
	FGameplayTagContainer CategoryTags;

	/**
	 * Icon used in the UI to represent this category.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Category")
	TSoftObjectPtr<UTexture> DisplayIcon;
	
	/**
	 * Optional bitmask for category-specific behaviors (e.g., Craftable, Equipable).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Category", 
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags"))
	uint8 CategoryFlags;

	/**
	 * Defines a list of allowed Actions.
	 * Each Item of this category can perform those Actions. Some Actions are not allowed in User Interface!
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory Category",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionInterface"))
	TArray<TSoftClassPtr<UMounteaInventoryItemAction>> AllowedActions;	
};

/**
 * FInventoryCategory represents a complete item category with hierarchical subcategory support.
 * Categories provide primary classification for inventory items with nested subcategory structures
 * for detailed item organization and management within the inventory system.
 *
 * @see [Item Categories](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemCategories)
 * @see FInventoryCategoryData
 * @see UMounteaInventoryItemTemplate
 */
USTRUCT(BlueprintType)
struct FInventoryCategory
{
	GENERATED_BODY()

	FInventoryCategory();

	/**
	 * Represents inventory category data, providing properties and metadata
	 * for managing and representing inventory category.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, 
		meta=(ShowOnlyInnerProperties))
	FInventoryCategoryData CategoryData;
		
	/**
	 * Array of localized subcategory names associated with the inventory category.
	 * Used for further categorization within a primary category, allowing more detailed item classification.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, 
		meta=(ForceInlineRow, ShowOnlyInnerProperties))
	TMap<FString,FInventoryCategoryData> SubCategories;
};

#undef LOCTEXT_NAMESPACE
#pragma endregion

#pragma region InventoryType
#define LOCTEXT_NAMESPACE "InventoryType"

/**
 * FInventoryTypeConfig defines the behavior, constraints, and presentation of different inventory types.
 * Type configurations control UI widgets, access permissions, capacity limits, weight systems,
 * and value restrictions for flexible inventory management across different use cases.
 *
 * @see [Inventory Types](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventoryTypes)
 * @see IMounteaAdvancedInventoryInterface
 * @see EInventoryFlags
 */
USTRUCT(BlueprintType)
struct FInventoryTypeConfig
{
	GENERATED_BODY()

	FInventoryTypeConfig();

	/** The name of the inventory (e.g., Player, Merchant, Loot), localized */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="User Interface")
	FText InventoryDisplayName = LOCTEXT("InventoryType_DisplayName", "");

	/** Widget class to use for this inventory type */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="User Interface", 
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	/** Configuration flags for this inventory type */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Configuration", 
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryTypeFlags"))
	uint8 ConfigFlags;

	/** Access and behavior flags for this inventory */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Configuration", 
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryFlags"))
	uint8 AccessFlags;

	/** Range of allowed slots (X = Min, Y = Max) */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Constrains", 
		meta=(ClampMin=1))
	FIntPoint SlotsRange = FIntPoint(10, 20);

	/** Starting number of slots (must be within SlotsRange) */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Constrains", 
		meta=(ClampMin=1, UIMin=1))
	int32 StartingSlots = 10;
	
	/** Maximum weight this inventory can hold */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Constrains", 
		meta=(ClampMin=0.0, Units="kg"))
	float MaxWeight = 100.0f;

	/** Thresholds for weight-based effects (in percentages) */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Constrains", 
		meta=(ClampMin=0.0, ClampMax=1.0, Units="Percent"))
	FVector4 WeightThresholds = FVector4(0.3f, 0.5f, 0.7f, 0.9f);

	/** Movement speed multipliers for each weight threshold */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Constrains", 
		meta=(ClampMin=0.0, ClampMax=1.0, Units="Percent"))
	FVector4 WeightSpeedMultipliers = FVector4(1.0f, 0.8f, 0.6f, 0.4f);

	/** Maximum total value this inventory can hold */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Constrains", 
		meta=(ClampMin=0.0))
	float MaxValue = 1000.0f;

	/** Tags defining special properties or restrictions for this inventory type */
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer InventoryTags;

public:

	bool HasWeightLimit() const;
	bool HasValueLimit() const;
	bool CanAddItems() const;

public:
	/** Access Flag Checks */
	bool IsPublic() const;
	bool IsTeamShared() const;
	bool IsLootable() const;
	bool IsTemporary() const;
	bool IsPrivate() const;

	/** Access Flag Setters */
	void SetPublic(const bool bValue);
	void SetTeamShared(const bool bValue);
	void SetLootable(const bool bValue);
	void SetTemporary(const bool bValue);
	void SetPrivate(const bool bValue);

	/** General Flag Manipulation */
	void SetFlag(const EInventoryFlags Flag, const bool bValue);
	bool HasFlag(const EInventoryFlags Flag) const;
};

#undef LOCTEXT_NAMESPACE
#pragma endregion

#pragma region ItemActionData
#define LOCTEXT_NAMESPACE "MounteaInventoryItemActionData"

/**
 * Data structure for defining item action properties.
 * Used to configure actions in the editor and at runtime.
 */
USTRUCT(BlueprintType)
struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaItemActionData
{
	GENERATED_BODY()

	FMounteaItemActionData() : 
		ActionDisplayName(LOCTEXT("DefaultActionName", "Default Action")),
		ActionDescription(LOCTEXT("DefaultActionDescription", "A basic inventory item action")),
		ActionPriority(0),
		bIsVisibleByDefault(true),
		bRequiresConfirmation(false),
		InventoryItemActionCallback(0)
	{}

public:
	
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
	TSoftObjectPtr<UTexture2D> Icon;

	/**
	 * Priority value used for sorting actions in UI elements.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	int32 ActionPriority = INDEX_NONE;

	/**
	 * Whether this action should be visible by default in the UI.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration")
	uint8 bIsVisibleByDefault : 1;

	/**
	 * Whether this action requires confirmation before execution.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration")
	uint8 bRequiresConfirmation : 1;

	/**
	 * The tag used to identify this action in the gameplay ability system.
	 * This tag is used for filtering and identifying actions in the UI and logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration")
	FGameplayTag ItemActionTag;

	/**
	 * Callback type for this action, used to determine how the action is processed.
	 * This can be used to trigger different behaviors based on the action type.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemActionCallback"))
	uint8 InventoryItemActionCallback;

	/**
	 * Gameplay effects to apply when this action is successfully executed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Configuration",
		DisplayName="Action Effects (GAS)")
	TArray<TSoftClassPtr<UGameplayEffect>> ActionEffects;
};

#undef LOCTEXT_NAMESPACE
#pragma endregion 

#pragma region InventorySortCriteria

/**
 * Defines a single sorting rule used to order items in an inventory.
 *
 * Each instance represents one criterion (sorting key) and its priority
 * relative to other criteria. When multiple sorting criteria are provided,
 * they are applied sequentially in ascending SortPriority order, where
 * lower values indicate higher priority.
 *
 * Example usage:
 * - SortPriority = 0, SortingKey = "Rarity"
 * - SortPriority = 1, SortingKey = "Value"
 * - SortPriority = 2, SortingKey = "Name"
 *
 * In this case, items are first sorted by Rarity, then by Value for items
 * with equal rarity, and finally by Name as a tie-breaker.
 *
 * Supported sorting keys typically include (but are not limited to):
 * - Name
 * - Value
 * - Weight
 * - Rarity
 */
USTRUCT(BlueprintType)
struct FInventorySortCriteria
{
	GENERATED_BODY()
	
public:
	
	// If more than 1 sorting criteria are provided, sorting will happen one after another based on priority.
	UPROPERTY(EditAnywhere)
	int32 SortPriority = 0;
	
	/* 
	 * Defines what sorting criteria there are. 
	 * Defaults:
	 * * Name
	 * * Value
	 * * Weight
	 * * Rarity
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SortingKey;
	
};

#pragma endregion

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
	return LHS.CategoryData.CategoryDisplayName.EqualTo(RHS.CategoryData.CategoryDisplayName) &&
		   LHS.CategoryData.CategoryPriority == RHS.CategoryData.CategoryPriority &&
		   LHS.CategoryData.CategoryTags == RHS.CategoryData.CategoryTags &&
		   LHS.CategoryData.DisplayIcon == RHS.CategoryData.DisplayIcon &&
		   LHS.CategoryData.CategoryFlags == RHS.CategoryData.CategoryFlags;
}

// Inequality operator for FInventoryCategory
FORCEINLINE bool operator!=(const FInventoryCategory& LHS, const FInventoryCategory& RHS)
{
	return !(LHS == RHS);
}