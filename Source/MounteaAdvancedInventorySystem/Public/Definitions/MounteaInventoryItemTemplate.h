// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"
#include "Styling/SlateBrush.h"

#include "MounteaInventoryItemTemplate.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemTemplate"

class UTexture;
enum class EInventoryItemFlags : uint8;

/**
 * Inventory Item Template class defining all properties of an inventory item.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, meta=(DisplayName = "Inventory Item Template"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaInventoryItemTemplate();

public:

	/** Primary Data **/

	/** A globally unique identifier for this item template. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data", DuplicateTransient)
	FGuid Guid;

	/** The item’s name, displayed in-game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FText DisplayName = LOCTEXT("MounteaInventoryItemTemplate_DisplayName", "");

	/** Reference to the item’s specific category (e.g., Weapon, Armor, Consumable). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data", meta=(GetOptions="GetAllowedCategories"))
	FString ItemCategory;

	/**
	 * Represents the subcategory of the inventory item, which provides additional classification beyond the main category.
	 * Used to further organize and identify items, such as distinguishing between different types within the same category.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data", meta=(GetOptions="GetAllowedSubCategories"))
	FString ItemSubCategory = TEXT("");

	/** Reference to the item’s rarity (e.g., Common, Rare, Epic). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data", meta=(GetOptions="GetAllowedRarities"))
	FString ItemRarity;

	/** A bitmask to define item properties such as tradeable, stackable, or consumable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data", meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags"))
	uint8 ItemFlags;

	/** Maximum amount of this item that can be held in a single inventory slot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data", meta=(UIMin=0,ClampMin=0))
	int32 MaxQuantity;

	/** Maximum stack size for this item in the inventory. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data", meta=(UIMin=1,ClampMin=1))
	int32 MaxStackSize;

	/** Tags for categorization, filtering, or triggering gameplay logic. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FGameplayTagContainer Tags;

	/** The actor spawned when the item is used, dropped, or equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	TSoftClassPtr<AActor> SpawnActor;

	/** Secondary Data **/

	/** A brief description of the item’s purpose or lore. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	FText ItemShortInfo=  LOCTEXT("MounteaInventoryItemTemplate_ItemShortInfo", "");;

	/** A detailed description providing additional lore or functionality. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(MultiLine=true))
	FText ItemLongInfo = LOCTEXT("MounteaInventoryItemTemplate_ItemLongInfo", "");;

	/** A small icon representing the item in the inventory UI. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	TSoftObjectPtr<UTexture2D> ItemThumbnail;

	/** A larger visual representation of the item, used for detailed views. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	TSoftObjectPtr<UTexture2D> ItemCover;

	/** The 3D mesh associated with the item (StaticMesh or SkeletalMesh). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	TObjectPtr<UStreamableRenderAsset> ItemMesh;

	/** Whether this item supports durability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	bool bHasDurability;

	/** The maximum durability value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasDurability"), meta=(UIMin=1,ClampMin=1))
	float MaxDurability;

	/** The starting durability value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasDurability"), meta=(UIMin=0,ClampMin=0))
	float BaseDurability;

	/** The penalty applied to durability under certain conditions (e.g., usage). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasDurability"), meta=(UIMin=0,ClampMin=0))
	float DurabilityPenalization;

	/** The coefficient affecting the item's price based on its durability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasDurability"), meta=(UIMin=0,ClampMin=0))
	float DurabilityToPriceCoefficient;

	/** Whether the item has an associated monetary value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	bool bHasPrice;

	/** The base monetary value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasPrice"), meta=(UIMin=0,ClampMin=0))
	float BasePrice;

	/** A multiplier applied to the base price when selling the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasPrice"), meta=(UIMin=0,ClampMin=0))
	float SellPriceCoefficient;

	/** Whether the item has weight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	bool bHasWeight;

	/** The weight of the item, which may affect inventory limits. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data", meta=(EditCondition="bHasWeight"), meta=(UIMin=0.001,ClampMin=0.001))
	float Weight;

	/** Defines the compatible slots or affector types for this item template. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	TSet<FGameplayTag> AffectorSlots;

	/** A reference to a special gameplay ability or effect triggered by this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	TObjectPtr<UObject> ItemSpecialAffect;

protected:

	UFUNCTION()
	static TArray<FString> GetAllowedCategories();
	UFUNCTION()
	TArray<FString> GetAllowedSubCategories() const;
	UFUNCTION()
	static TArray<FString> GetAllowedRarities();

#if WITH_EDITOR
	
protected:

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
	
};

#undef LOCTEXT_NAMESPACE
