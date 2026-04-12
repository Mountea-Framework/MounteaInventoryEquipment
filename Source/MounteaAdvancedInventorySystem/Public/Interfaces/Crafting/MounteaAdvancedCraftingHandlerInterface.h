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
#include "UObject/Interface.h"
#include "MounteaAdvancedCraftingHandlerInterface.generated.h"

class UMounteaRecipeTemplate;
class UMounteaRecipeIngredientsList;

UINTERFACE()
class UMounteaAdvancedCraftingHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedCraftingHandlerInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool IsCraftingPossible(UMounteaRecipeTemplate* TemplateToCraft);
	virtual bool IsCraftingPossible_Implementation(UMounteaRecipeTemplate* TemplateToCraft) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	FMounteaCraftingResult StartCrafting(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
	virtual FMounteaCraftingResult StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients) = 0;
};
