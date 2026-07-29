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
#include "MounteaAdvancedCraftingUIConfig.generated.h"

/**
 * UMounteaAdvancedCraftingUIConfig is a data asset managing crafting UI widget classes.
 * Settings define the main crafting screen, recipe lists, and recipe details widgets.
 *
 * @see [Settings Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Crafting UI Settings Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining crafting UI."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedCraftingUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	// ---- Crafting
	
	/** Widget class used to display the crafting screen itself. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingUserInterfaceClass;
	
	// ---- Categories
	
	/** Widget class used as a container for all crafting categories. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Categories",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoriesWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingCategoriesUserInterfaceClass;
	
	/** Widget class representing a single crafting category entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Categories",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingCategoryUserInterfaceClass;
	
	// ---- Recipes
	
	/** Widget class used as a container for craftable recipe widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipes",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipesWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipesUserInterfaceClass;
	
	/** Widget class representing a single craftable recipe entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipes",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeUserInterfaceClass;
	
	// ---- Recipe Details
	
	/** Widget class used as the wrapper for recipe details child widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe Details",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeDetailsPanelWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeDetailsPanelUserInterfaceClass;
	
	/** Widget class used as a container for all ingredients in a selected recipe ingredient option. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe Details",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeIngredientsWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeIngredientsWrapperUserInterfaceClass;
	
	/** Widget class representing a single recipe ingredient. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe Details",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeIngredientWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeIngredientUserInterfaceClass;
	
	/** Widget class representing the result item of the crafting process. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe Details",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeResultWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeResultUserInterfaceClass;
	
	/** Widget class used to display information read from the item being created. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe Details",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeInfoboxWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeInfoboxUserInterfaceClass;
	
	/** Widget class used as a wrapper for the crafting amount slider element. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe Details",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCraftingRecipeSliderWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CraftingRecipeSliderUserInterfaceClass;
};
