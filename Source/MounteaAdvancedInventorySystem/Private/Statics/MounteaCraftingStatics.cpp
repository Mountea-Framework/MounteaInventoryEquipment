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


#include "Statics/MounteaCraftingStatics.h"

#include "Definitions/MounteaCraftingBaseDataTypes.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingHandlerInterface.h"

bool UMounteaCraftingStatics::IsCraftingPossible(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft)
{
	return IsValid(Target) ? IMounteaAdvancedCraftingHandlerInterface::Execute_IsCraftingPossible(Target, TemplateToCraft) : false;
}

FMounteaCraftingResult UMounteaCraftingStatics::StartCrafting(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	return IMounteaAdvancedCraftingHandlerInterface::Execute_StartCrafting(Target, TemplateToCraft, Ingredients);
}

bool UMounteaCraftingStatics::CraftItem(const TScriptInterface<IMounteaAdvancedCraftingHandlerInterface>& Target,
	UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	// TODO:
	// get inventory from Target
	// get items from inventory
	// prepare payload
	// consume from inventory
	// if fails, break
	// if success, add new item
	// if item add fails, break
	// otherwise all was good, return true
	
	return false;
}
