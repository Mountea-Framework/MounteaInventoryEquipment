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
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"
#include "Styling/SlateBrush.h"

#include "MounteaInventoryItemTemplate.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemTemplate"

class UTexture;
class UMounteaSelectableInventoryItemAction;
enum class EInventoryItemFlags : uint8;

/**
 * UMounteaInventoryItemTemplate is a data asset that defines the static properties of inventory items.
 * Item templates serve as blueprints for creating inventory item instances, containing all base data
 * such as display information, categories, flags, durability settings, and gameplay tags.
 *
 * @see [Item Templates](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemTemplates)
 * @see FInventoryItem
 * @see UMounteaInventoryManagerComponent
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data", DuplicateTransient,
		meta=(NoResetToDefault))
	FGuid Guid;

	/** The item’s name, displayed in-game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault))
	FText DisplayName = LOCTEXT("MounteaInventoryItemTemplate_DisplayName", "");

	/** Reference to the item’s specific category (e.g., Weapon, Armor, Consumable). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(GetOptions="GetAllowedCategories"), meta=(NoResetToDefault))
	FString ItemCategory;

	/**
	 * Represents the subcategory of the inventory item, which provides additional classification beyond the main category.
	 * Used to further organize and identify items, such as distinguishing between different types within the same category.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(GetOptions="GetAllowedSubCategories"), meta=(NoResetToDefault))
	FString ItemSubCategory = TEXT("");

	/** Reference to the item’s rarity (e.g., Common, Rare, Epic). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(GetOptions="GetAllowedRarities"), meta=(NoResetToDefault))
	FString ItemRarity;

	/** A bitmask to define item properties such as tradeable, stackable, or consumable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags"),
		meta=(NoResetToDefault))
	uint8 ItemFlags;

	/** Maximum amount of this item that can be held in a single inventory slot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(UIMin=0,ClampMin=0), meta=(NoResetToDefault))
	int32 MaxQuantity;

	/** Maximum stack size for this item in the inventory. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(UIMin=1,ClampMin=1), meta=(NoResetToDefault))
	int32 MaxStackSize;

	/** Tags for categorization, filtering, or triggering gameplay logic. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault))
	FGameplayTagContainer Tags;

	/** The actor spawned when the item is used, dropped, or equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault))
	TSoftClassPtr<AActor> SpawnActor;

	/** Secondary Data **/

	/** A brief description of the item’s purpose or lore. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	FText ItemShortInfo = LOCTEXT("MounteaInventoryItemTemplate_ItemShortInfo", "");;

	/** A detailed description providing additional lore or functionality. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(MultiLine=true), meta=(NoResetToDefault))
	FText ItemLongInfo = LOCTEXT("MounteaInventoryItemTemplate_ItemLongInfo", "");;

	/** A small icon representing the item in the inventory UI. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	TSoftObjectPtr<UTexture2D> ItemThumbnail;

	/** A larger visual representation of the item, used for detailed views. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	TSoftObjectPtr<UTexture2D> ItemCover;

	/** The 3D mesh associated with the item (StaticMesh or SkeletalMesh). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	TObjectPtr<UStreamableRenderAsset> ItemMesh;

	/** Whether this item supports durability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data")
	bool bHasDurability;

	/** The maximum durability value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=1,ClampMin=1), meta=(NoResetToDefault))
	float MaxDurability;

	/** The starting durability value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=0,ClampMin=0))
	float BaseDurability;

	/** The penalty applied to durability under certain conditions (e.g., usage). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), meta=(NoResetToDefault))
	float DurabilityPenalization;

	/** The coefficient affecting the item's price based on its durability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), meta=(NoResetToDefault))
	float DurabilityToPriceCoefficient;

	/** Whether the item has an associated monetary value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	bool bHasPrice;

	/** The base monetary value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasPrice", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), meta=(NoResetToDefault))
	float BasePrice;

	/** A multiplier applied to the base price when selling the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasPrice", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), meta=(NoResetToDefault))
	float SellPriceCoefficient;

	/** Whether the item has weight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	bool bHasWeight;

	/** The weight of the item, which may affect inventory limits. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasWeight", EditConditionHides),
		meta=(UIMin=0.001,ClampMin=0.001), meta=(NoResetToDefault))
	float Weight;

	/** Defines the compatible slots or affector types for this item template. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault))
	FGameplayTagContainer AttachmentSlots;
	
	/**
	 * Definition of allowed Actions for specific Category.
	 * Each Item of this category can perform selected Action.
	 * 
	 * When Item is created, all Actions based on Item Category are pre-defined.
	 * When Category/Subcategory is changed, all Item Actions are removed!
	 * 
	 * Future enhancement:
	 * → Do not delete shared Item Actions
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Actions", Instanced)
	TArray<TObjectPtr<UMounteaSelectableInventoryItemAction>> ItemActions;

	/** A reference to a special gameplay abilities or effects triggered by this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionInterface"),
		meta=(NoResetToDefault))
	TSet<TSoftClassPtr<UObject>> ItemSpecialAffects;
	
protected:
	
	// JSON manifest of the Inventory Item.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Technical Data", AdvancedDisplay,
		meta=(Multiline), meta=(NoResetToDefault))
	FString JsonManifest;
	
	// Asset path which defines source location where the item is located.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Technical Data", AdvancedDisplay,
		meta=(NoResetToDefault))
	FFilePath ImportFilePath;
	
public:
	
	FString GetJson() const;
	void SetJson(const FString& Json);
	bool CalculateJson();
	
protected:

	UFUNCTION()
	static TArray<FString> GetAllowedCategories();
	UFUNCTION()
	TArray<FString> GetAllowedSubCategories() const;
	UFUNCTION()
	static TArray<FString> GetAllowedRarities();

#if WITH_EDITOR
	
public:
	
	/**
	 * Request default Item Actions from Category.
	 * This will delete all current actions and will create new ones based on selected Category.
	 * 
	 * This function is called anytime you change Category for the Item Template, press the button only
	 * if you messed up and need to re-load Item Actions!
	 */
	UFUNCTION(CallInEditor, Category="Edit Actions")
	virtual void ReloadItemActions();
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemTemplateChanged, UMounteaInventoryItemTemplate*);
	FOnItemTemplateChanged TemplateChangedDelegate;
	
protected:

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
};

#undef LOCTEXT_NAMESPACE
