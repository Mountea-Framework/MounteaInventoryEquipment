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
		meta=(NoResetToDefault),
		meta=(DisplayPriority=0))
	FGuid Guid;

	/** The item’s name, displayed in-game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=1))
	FText DisplayName = LOCTEXT("MounteaInventoryItemTemplate_DisplayName", "");

	/** Reference to the item’s specific category (e.g., Weapon, Armor, Consumable). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(GetOptions="GetAllowedCategories"), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=2))
	FString ItemCategory;

	/**
	 * Represents the subcategory of the inventory item, which provides additional classification beyond the main category.
	 * Used to further organize and identify items, such as distinguishing between different types within the same category.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(GetOptions="GetAllowedSubCategories"), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=3))
	FString ItemSubCategory = TEXT("");

	/** Reference to the item’s rarity (e.g., Common, Rare, Epic). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(GetOptions="GetAllowedRarities"), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=4))
	FString ItemRarity;

	/** Maximum amount of this item that can be held in a single inventory slot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(UIMin=0,ClampMin=0), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=5))
	int32 MaxQuantity;

	/** Maximum stack size for this item in the inventory. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(UIMin=1,ClampMin=1), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=6))
	int32 MaxStackSize;

	/** A bitmask to define item properties such as tradeable, stackable, or consumable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemFlags"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=7))
	uint8 ItemFlags;

	/** Tags for categorization, filtering, or triggering gameplay logic. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=8))
	FGameplayTagContainer Tags;

	/** The actor spawned when the item is used, dropped, or equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=9))
	TSoftClassPtr<AActor> SpawnActor;

	/** Secondary Data **/

	/** A brief description of the item’s purpose or lore. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=10))
	FText ItemShortInfo = LOCTEXT("MounteaInventoryItemTemplate_ItemShortInfo", "");;

	/** A detailed description providing additional lore or functionality. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(MultiLine=true), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=11))
	FText ItemLongInfo = LOCTEXT("MounteaInventoryItemTemplate_ItemLongInfo", "");;

	/** A small icon representing the item in the inventory UI. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=12))
	TSoftObjectPtr<UTexture2D> ItemThumbnail;

	/** A larger visual representation of the item, used for detailed views. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=13))
	TSoftObjectPtr<UTexture2D> ItemCover;

	/** The 3D mesh associated with the item (StaticMesh or SkeletalMesh). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=14))
	TObjectPtr<UStreamableRenderAsset> ItemMesh;

	/** Whether this item supports durability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=15))
	bool bHasDurability;

	/** The maximum durability value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=1,ClampMin=1), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=16))
	float MaxDurability;

	/** The starting durability value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=0,ClampMin=0),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=17))
	float BaseDurability;

	/** The penalty applied to durability under certain conditions (e.g., usage). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=18))
	float DurabilityPenalization;

	/** The coefficient affecting the item's price based on its durability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasDurability", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=19))
	float DurabilityToPriceCoefficient;

	/** Whether the item has an associated monetary value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=20))
	bool bHasPrice;

	/** The base monetary value of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasPrice", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=21))
	float BasePrice;

	/** A multiplier applied to the base price when selling the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasPrice", EditConditionHides),
		meta=(UIMin=0,ClampMin=0), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=22))
	float SellPriceCoefficient;

	/** Whether the item has weight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=23))
	bool bHasWeight;

	/** The weight of the item, which may affect inventory limits. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(EditCondition="bHasWeight", EditConditionHides),
		meta=(UIMin=0.001,ClampMin=0.001), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=24))
	float Weight;

	/** Defines the compatible slots this template can be attached to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(NoResetToDefault),
		meta=(DisplayPriority=25))
	FGameplayTagContainer AttachmentSlots;
	
	/**
	 * Definition of allowed Actions for specific Category.
	 * Each Item of this category can perform selected Action.
	 * 
	 * When Item is created, all Actions based on Item Category are pre-defined.
	 * When Category/Subcategory is changed, all Item Actions are removed!
	 * 
	 * Future enhancement:
	 * - Do not delete shared Item Actions
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Actions", Instanced,
		meta=(NoResetToDefault),
		meta=(DisplayPriority=26))
	TArray<TObjectPtr<UMounteaSelectableInventoryItemAction>> ItemActions;

	/** A reference to a special gameplay abilities or effects triggered by this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Secondary Data",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionInterface"),
		meta=(NoResetToDefault),
		meta=(DisplayPriority=27))
	TSet<TSoftClassPtr<UObject>> ItemSpecialAffects;
	
protected:
	
	// JSON manifest of the Inventory Item.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Technical Data", AdvancedDisplay,
		meta=(Multiline), 
		meta=(NoResetToDefault),
		meta=(DisplayPriority=28))
	FString JsonManifest;
	
	// Asset path which defines source location where the item is located.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Technical Data", AdvancedDisplay,
		meta=(NoResetToDefault),
		meta=(DisplayPriority=29))
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