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
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaRecipeIngredientsList.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingStationInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Settings/MounteaAdvancedCraftingConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

/**
 * CRAFTING PARTICIPANT
 */

bool UMounteaCraftingStatics::IsValidRecipeHandler(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedCraftingParticipantInterface>();
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::GetKnownRecipes(UObject* Target)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_GetKnownRecipes(Target) : TArray<UMounteaRecipeTemplate*>();
}

UMounteaRecipeTemplate* UMounteaCraftingStatics::GetRecipe(UObject* Target, const FGuid& RecipeGuid)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_GetRecipe(Target, RecipeGuid) : nullptr;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::GetRecipes(UObject* Target, const FGameplayTag& CraftingStationType)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_GetRecipes(Target, CraftingStationType) : TArray<UMounteaRecipeTemplate*>();
}

bool UMounteaCraftingStatics::IsRecipeKnown(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_IsRecipeKnown(Target, RecipeTemplate) : false;
}

bool UMounteaCraftingStatics::LearnRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Target, RecipeTemplate) : false;
}

bool UMounteaCraftingStatics::ForgetRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_ForgetRecipe(Target, RecipeTemplate) : false;
}

bool UMounteaCraftingStatics::IsCraftingPossible(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_IsCraftingPossible(Target, TemplateToCraft) : false;
}

FMounteaCraftingResult UMounteaCraftingStatics::StartCrafting(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	return IMounteaAdvancedCraftingParticipantInterface::Execute_StartCrafting(Target, TemplateToCraft, Ingredients);
}

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaCraftingStatics::GetParentInventory_Implementation(UObject* Target)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_GetParentInventory(Target) : TScriptInterface<IMounteaAdvancedInventoryInterface>();
}

bool UMounteaCraftingStatics::SetParentInventory_Implementation(UObject* Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_SetParentInventory(Target, NewParentInventory) : false;
}

TSet<UMounteaRecipeTemplate*> UMounteaCraftingStatics::GetAllRecipeTemplates()
{
	const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings))
		return TSet<UMounteaRecipeTemplate*>();
	const auto craftingConfig = settings->AdvancedCraftingSettingsConfig.LoadSynchronous();
	if (!IsValid(craftingConfig))
		return TSet<UMounteaRecipeTemplate*>();
	const auto& allowedRecipes = craftingConfig->AllowedRecipes;
	
	TSet<UMounteaRecipeTemplate*> returnValue;
	returnValue.Reserve(allowedRecipes.Num());
	
	for(const auto& recipe : allowedRecipes)
	{
		returnValue.Add(recipe.Get());
	}
	
	return returnValue;
}

FMounteaCraftingResult UMounteaCraftingStatics::CraftItem(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	FMounteaCraftingResult result;

	if (!Target || !IsValid(TemplateToCraft) || !IsValid(Ingredients))
		return result;

	const TScriptInterface<IMounteaAdvancedInventoryInterface> inventory =
		IMounteaAdvancedCraftingParticipantInterface::Execute_GetParentInventory(Target.GetObject());
	if (!inventory)
		return result;

	UMounteaInventoryItemTemplate* resultTemplate = TemplateToCraft->ResultItem.LoadSynchronous();
	if (!IsValid(resultTemplate))
		return result;

	for (const UMounteaRecipeIngredient* ingredient : Ingredients->RecipeIngredients)
	{
		if (!IsValid(ingredient))
			return result;

		UMounteaInventoryItemTemplate* ingredientTemplate = ingredient->IngredientSource.LoadSynchronous();
		if (!IsValid(ingredientTemplate))
			return result;

		const TArray<FMounteaInventoryItem> foundItems =
			IMounteaAdvancedInventoryInterface::Execute_FindItems(inventory.GetObject(), FInventoryItemSearchParams(ingredientTemplate));

		int32 totalQuantity = 0;
		for (const FMounteaInventoryItem& item : foundItems)
			totalQuantity += item.Quantity;

		if (totalQuantity < ingredient->RequiredQuantity)
			return result;
	}

	if (!IMounteaAdvancedInventoryInterface::Execute_CanAddItemFromTemplate(inventory.GetObject(), resultTemplate, TemplateToCraft->QuantityPerCreation))
		return result;

	for (const UMounteaRecipeIngredient* ingredient : Ingredients->RecipeIngredients)
	{
		UMounteaInventoryItemTemplate* ingredientTemplate = ingredient->IngredientSource.LoadSynchronous();
		if (!IMounteaAdvancedInventoryInterface::Execute_RemoveItemFromTemplate(inventory.GetObject(), ingredientTemplate, ingredient->RequiredQuantity))
			return result;
	}

	if (!IMounteaAdvancedInventoryInterface::Execute_AddItemFromTemplate(inventory.GetObject(), resultTemplate, TemplateToCraft->QuantityPerCreation, 1))
		return result;

	result.bCraftingSuccess = true;
	return result;
}

/**
 * CRAFTING PLACE
 */

bool UMounteaCraftingStatics::IsValidCraftingPlace(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedCraftingStationInterface>();
}

FGameplayTag UMounteaCraftingStatics::GetCraftingPlaceType(UObject* Target)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_GetCraftingPlaceType(Target) : FGameplayTag();
}

bool UMounteaCraftingStatics::IsCraftingPlaceOccupied(UObject* Target)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_IsCraftingPlaceOccupied(Target) : false;
}

int32 UMounteaCraftingStatics::GetCraftingPlaceCapacity(UObject* Target)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_GetCraftingPlaceCapacity(Target) : INDEX_NONE;
}
