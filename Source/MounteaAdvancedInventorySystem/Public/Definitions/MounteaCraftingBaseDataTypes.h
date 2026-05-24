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
#include "UObject/Object.h"
#include "MounteaCraftingBaseDataTypes.generated.h"

class UMounteaRecipeTemplate;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMounteaCraftingResult
{
	GENERATED_BODY()
	
	FMounteaCraftingResult() 
		: bCraftingSuccess(false),
		ResultItemId(FGuid::NewGuid())
	{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Crafting")
	uint8 bCraftingSuccess : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Crafting")
	FGuid ResultItemId;	
};

/**
 * Search parameters for filtering known crafting recipes.
 * Filters can be stacked and are expected to run in this order:
 * 1) Available ingredients
 * 2) Station type
 * 3) Recipe source
 */
USTRUCT(BlueprintType)
struct FMounteaCraftingRecipeSearchFilter
{
	GENERATED_BODY()

	FMounteaCraftingRecipeSearchFilter()
		: bSearchByStationType(false)
		, bSearchByAvailableIngredients(false)
		, bSearchByRecipeSource(false)
		, RecipeSource(nullptr)
	{
	}

	explicit FMounteaCraftingRecipeSearchFilter(const FGameplayTag& InStationType)
		: bSearchByStationType(true)
		, StationType(InStationType)
		, bSearchByAvailableIngredients(false)
		, bSearchByRecipeSource(false)
		, RecipeSource(nullptr)
	{
	}

	explicit FMounteaCraftingRecipeSearchFilter(UMounteaRecipeTemplate* InRecipeSource)
		: bSearchByStationType(false)
		, bSearchByAvailableIngredients(false)
		, bSearchByRecipeSource(true)
		, RecipeSource(InRecipeSource)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByStationType : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search",
		meta=(Categories="Mountea_Inventory.Crafting,Crafting"))
	FGameplayTag StationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByAvailableIngredients : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByRecipeSource : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	TObjectPtr<UMounteaRecipeTemplate> RecipeSource;
};
