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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaCraftingStatics.generated.h"

class UMounteaRecipeTemplate;
class UMounteaRecipeIngredientsList;
class IMounteaAdvancedCraftingHandlerInterface;

struct FMounteaCraftingResult;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Helpers",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Is Crafting Possible")
	static bool IsCraftingPossible(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Helpers",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Start Crafting")
	static FMounteaCraftingResult StartCrafting(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
	
public:
	
	static bool CraftItem(const TScriptInterface<IMounteaAdvancedCraftingHandlerInterface>& Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
};
