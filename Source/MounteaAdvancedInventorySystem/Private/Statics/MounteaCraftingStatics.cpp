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

#include "Algo/Copy.h"
#include "Algo/ForEach.h"
#include "Algo/Transform.h"
#include "Definitions/MounteaCraftingBaseDataTypes.h"
#include "Definitions/MounteaCraftingBaseEnums.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaInventoryItemTemplate_Recipe.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaRecipeIngredientsList.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingStationInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Settings/MounteaAdvancedCraftingConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Statics/MounteaInventoryStatics.h"

#define MOUNTEA_BIND_CRAFTING_DELEGATE(Target, Binding, HandleGetter) \
	if (!IsValid((Target).GetObject()) || !(Binding).IsBound()) \
		return false; \
	IMounteaAdvancedCraftingParticipantInterface* nativeInterface = Cast<IMounteaAdvancedCraftingParticipantInterface>((Target).GetObject()); \
	if (!nativeInterface) \
		return false; \
	nativeInterface->HandleGetter().AddUnique(Binding); \
	return true

#define MOUNTEA_UNBIND_CRAFTING_DELEGATE(Target, Binding, HandleGetter) \
	if (!IsValid((Target).GetObject()) || !(Binding).IsBound()) \
		return false; \
	IMounteaAdvancedCraftingParticipantInterface* nativeInterface = Cast<IMounteaAdvancedCraftingParticipantInterface>((Target).GetObject()); \
	if (!nativeInterface) \
		return false; \
	nativeInterface->HandleGetter().Remove(Binding); \
	return true

bool UMounteaCraftingStatics::HasInventoryItemForRecipeSourceCached(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	UMounteaInventoryItemTemplate_Recipe* RecipeSource,
	TMap<UMounteaInventoryItemTemplate_Recipe*, bool>& RecipeSourcePresenceCache)
{
	if (!IsValid(RecipeSource))
		return false;

	if (const bool* cachedPresence = RecipeSourcePresenceCache.Find(RecipeSource))
		return *cachedPresence;

	const bool bHasRecipeItem = UMounteaInventoryStatics::FindItems(
		Inventory,
		FInventoryItemSearchParams(RecipeSource)).Num() > 0;

	RecipeSourcePresenceCache.Add(RecipeSource, bHasRecipeItem);
	return bHasRecipeItem;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::ApplyRecipeSourceGrantFilter(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const TArray<UMounteaRecipeTemplate*>& SourceRecipes)
{
	TMap<UMounteaInventoryItemTemplate_Recipe*, bool> recipeSourcePresenceCache;
	
	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(SourceRecipes.Num());
	Algo::CopyIf(SourceRecipes, result,
		[&Inventory, &recipeSourcePresenceCache](const UMounteaRecipeTemplate* recipe) -> bool
		{
			if (!IsValid(recipe))
				return false;

			if (recipe->RecipeSource.IsNull())
				return true;

			UMounteaInventoryItemTemplate_Recipe* recipeSource = recipe->RecipeSource.LoadSynchronous();
			if (!IsValid(recipeSource))
			{
				// invalid RecipeSource asset is treated as unavailable source and filtered out.
				return false;
			}

			if (!recipeSource->bTemporaryGrant)
				return true;

			return HasInventoryItemForRecipeSourceCached(Inventory, recipeSource, recipeSourcePresenceCache);
		});

	return result;
}

int32 UMounteaCraftingStatics::GetIngredientQuantityCached(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	UMounteaInventoryItemTemplate* IngredientTemplate,
	TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache)
{
	if (!IsValid(IngredientTemplate))
		return 0;

	if (const int32* cachedQuantity = IngredientQuantityCache.Find(IngredientTemplate))
		return *cachedQuantity;

	const TArray<FMounteaInventoryItem> matchingItems =
		IMounteaAdvancedInventoryInterface::Execute_FindItems(
			Inventory.GetObject(),
			FInventoryItemSearchParams(IngredientTemplate));

	int32 totalQuantity = 0;
	for (const FMounteaInventoryItem& matchingItem : matchingItems)
		totalQuantity += matchingItem.Quantity;

	IngredientQuantityCache.Add(IngredientTemplate, totalQuantity);
	return totalQuantity;
}

bool UMounteaCraftingStatics::IsIngredientSatisfied(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const UMounteaRecipeIngredient* Ingredient,
	TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache)
{
	if (!IsValid(Ingredient))
		return false;

	UMounteaInventoryItemTemplate* ingredientTemplate = Ingredient->IngredientSource.LoadSynchronous();
	if (!IsValid(ingredientTemplate))
		return false;

	return GetIngredientQuantityCached(Inventory, ingredientTemplate, IngredientQuantityCache) >= Ingredient->RequiredQuantity;
}

bool UMounteaCraftingStatics::IsIngredientGroupSatisfied(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const UMounteaRecipeIngredientsList* IngredientGroup,
	TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache)
{
	if (!IsValid(IngredientGroup))
	{
		// invalid ingredient groups are treated as non-craftable options and skipped.
		return false;
	}

	return !IngredientGroup->RecipeIngredients.ContainsByPredicate(
		[&Inventory, &IngredientQuantityCache](const UMounteaRecipeIngredient* ingredient) -> bool
		{
			return !IsIngredientSatisfied(Inventory, ingredient, IngredientQuantityCache);
		});
}

bool UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const UMounteaRecipeTemplate* Recipe,
	TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache)
{
	if (!IsValid(Recipe))
		return false;

	if (Recipe->RecipeIngredientOptions.Num() == 0)
		return true;

	return Recipe->RecipeIngredientOptions.ContainsByPredicate(
		[&Inventory, &IngredientQuantityCache](const UMounteaRecipeIngredientsList* ingredientGroup) -> bool
		{
			return IsIngredientGroupSatisfied(Inventory, ingredientGroup, IngredientQuantityCache);
		});
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::ApplyIngredientAvailabilityFilter(
	const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
	const TArray<UMounteaRecipeTemplate*>& SourceRecipes)
{
	TMap<UMounteaInventoryItemTemplate*, int32> ingredientQuantityCache;

	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(SourceRecipes.Num());
	Algo::CopyIf(SourceRecipes, result,
		[&Inventory, &ingredientQuantityCache](const UMounteaRecipeTemplate* recipe) -> bool
		{
			return IsRecipeSatisfiedByIngredients(Inventory, recipe, ingredientQuantityCache);
		});

	return result;
}

bool UMounteaCraftingStatics::IsRecipeInCategory(
	const UMounteaRecipeTemplate* Recipe,
	const FGameplayTag& CategoryTag)
{
	if (!IsValid(Recipe) || !CategoryTag.IsValid())
		return false;

	const UMounteaInventoryItemTemplate* resultItem = Recipe->ResultItem.LoadSynchronous();
	if (!IsValid(resultItem))
		return false;

	const UMounteaAdvancedInventorySettings* inventorySettings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
		return false;

	const TMap<FString, FInventoryCategory>& allowedCategories = inventorySettings->GetAllowedCategories();
	const FInventoryCategory* allowedCategory = allowedCategories.Find(resultItem->ItemCategory);
	if (!allowedCategory)
		return false;

	return allowedCategory->CategoryData.CategoryTags.HasTag(CategoryTag);
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::ApplyCategoryFilter(
	const TArray<UMounteaRecipeTemplate*>& SourceRecipes,
	const FGameplayTag& CategoryTag)
{
	if (!CategoryTag.IsValid() || !IsAllowedInventoryCategoryTag(CategoryTag))
		return {};

	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(SourceRecipes.Num());
	Algo::CopyIf(SourceRecipes, result,
		[&CategoryTag](const UMounteaRecipeTemplate* recipe) -> bool
		{
			return IsRecipeInCategory(recipe, CategoryTag);
		});

	return result;
}

void UMounteaCraftingStatics::GetAllowedInventoryCategoryTags(TArray<FGameplayTag>& OutCategoryTags)
{
	OutCategoryTags.Empty();

	const UMounteaAdvancedInventorySettings* inventorySettings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(inventorySettings))
		return;

	const TMap<FString, FInventoryCategory>& allowedCategories = inventorySettings->GetAllowedCategories();
	FGameplayTagContainer allowedCategoryTags;
	Algo::ForEach(allowedCategories,
		[&allowedCategoryTags](const TPair<FString, FInventoryCategory>& allowedCategory)
		{
			allowedCategoryTags.AppendTags(allowedCategory.Value.CategoryData.CategoryTags);
		});

	allowedCategoryTags.GetGameplayTagArray(OutCategoryTags);
	OutCategoryTags.RemoveAll([](const FGameplayTag& categoryTag)
	{
		return !categoryTag.IsValid();
	});

	OutCategoryTags.Sort([](const FGameplayTag& Left, const FGameplayTag& Right)
	{
		return Left.ToString() < Right.ToString();
	});
}

bool UMounteaCraftingStatics::IsAllowedInventoryCategoryTag(const FGameplayTag& CategoryTag)
{
	if (!CategoryTag.IsValid())
		return false;

	TArray<FGameplayTag> allowedCategoryTags;
	GetAllowedInventoryCategoryTags(allowedCategoryTags);

	return allowedCategoryTags.Contains(CategoryTag);
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::ApplyStationTypeFilter(
	const TArray<UMounteaRecipeTemplate*>& SourceRecipes,
	const FGameplayTag& StationType)
{
	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(SourceRecipes.Num());
	Algo::CopyIf(SourceRecipes, result,
		[&StationType](const UMounteaRecipeTemplate* recipe) -> bool
		{
			return IsValid(recipe)
				&& (!recipe->RequiredCraftingPlace.IsValid() || recipe->RequiredCraftingPlace == StationType);
		});

	return result;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::ApplyRecipeSourceFilter(
	const TArray<UMounteaRecipeTemplate*>& SourceRecipes,
	const UMounteaRecipeTemplate* RecipeSource)
{
	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(SourceRecipes.Num());
	Algo::CopyIf(SourceRecipes, result,
		[RecipeSource](const UMounteaRecipeTemplate* recipe) -> bool
		{
			return recipe == RecipeSource;
		});

	return result;
}

/**
 * CRAFTING PARTICIPANT
 */

bool UMounteaCraftingStatics::IsValidRecipeHandler(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedCraftingParticipantInterface>();
}

bool UMounteaCraftingStatics::BindToOnCraftingFinished(
	const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target,
	const FMounteaCraftingFinishedBinding& Binding)
{
	MOUNTEA_BIND_CRAFTING_DELEGATE(Target, Binding, GetOnCraftingFinishedEventHandle);
}

bool UMounteaCraftingStatics::UnbindFromOnCraftingFinished(
	const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target,
	const FMounteaCraftingFinishedBinding& Binding)
{
	MOUNTEA_UNBIND_CRAFTING_DELEGATE(Target, Binding, GetOnCraftingFinishedEventHandle);
}

bool UMounteaCraftingStatics::BindToOnRecipeLearned(
	const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target,
	const FMounteaRecipeLearnedBinding& Binding)
{
	MOUNTEA_BIND_CRAFTING_DELEGATE(Target, Binding, GetOnRecipeLearnedEventHandle);
}

bool UMounteaCraftingStatics::UnbindFromOnRecipeLearned(
	const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target,
	const FMounteaRecipeLearnedBinding& Binding)
{
	MOUNTEA_UNBIND_CRAFTING_DELEGATE(Target, Binding, GetOnRecipeLearnedEventHandle);
}

bool UMounteaCraftingStatics::BindToOnRecipeForgotten(
	const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target,
	const FMounteaRecipeForgottenBinding& Binding)
{
	MOUNTEA_BIND_CRAFTING_DELEGATE(Target, Binding, GetOnRecipeForgottenEventHandle);
}

bool UMounteaCraftingStatics::UnbindFromOnRecipeForgotten(
	const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target,
	const FMounteaRecipeForgottenBinding& Binding)
{
	MOUNTEA_UNBIND_CRAFTING_DELEGATE(Target, Binding, GetOnRecipeForgottenEventHandle);
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::GetFilteredRecipes(UObject* Target, const FMounteaCraftingRecipeSearchFilter& SearchFilter)
{
	if (!IsValidRecipeHandler(Target))
		return {};

	const TArray<UMounteaRecipeTemplate*> knownRecipes = GetRecipes(Target, FGameplayTag::EmptyTag);
	if (knownRecipes.Num() == 0)
		return {};

	const TScriptInterface<IMounteaAdvancedInventoryInterface> parentInventory = GetParentInventory(Target);
	if (!parentInventory)
		return {};

	TArray<UMounteaRecipeTemplate*> filteredRecipes = ApplyRecipeSourceGrantFilter(parentInventory, knownRecipes);

	// Apply stacked filters in this order:
	// 1) ingredients
	// 2) category
	// 3) station type
	// 4) recipe source
	if (SearchFilter.bSearchByAvailableIngredients)
		filteredRecipes = ApplyIngredientAvailabilityFilter(parentInventory, filteredRecipes);

	if (SearchFilter.bSearchByCategory)
	{
		if (!SearchFilter.CategoryTag.IsValid())
			return {};

		filteredRecipes = ApplyCategoryFilter(filteredRecipes, SearchFilter.CategoryTag);
	}

	if (SearchFilter.bSearchByStationType)
	{
		if (!SearchFilter.StationType.IsValid())
			return {};
		
		filteredRecipes = ApplyStationTypeFilter(filteredRecipes, SearchFilter.StationType);
	}

	if (SearchFilter.bSearchByRecipeSource)
	{
		if (!IsValid(SearchFilter.RecipeSource))
			return {};
		
		filteredRecipes = ApplyRecipeSourceFilter(filteredRecipes, SearchFilter.RecipeSource);
	}

	return filteredRecipes;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::GetFilteredRecipesByCategory(UObject* Target, const FGameplayTag& CategoryTag)
{
	if (!CategoryTag.IsValid())
		return {};

	FMounteaCraftingRecipeSearchFilter searchFilter;
	searchFilter.bSearchByAvailableIngredients = true;
	searchFilter.bSearchByCategory = true;
	searchFilter.CategoryTag = CategoryTag;

	return GetFilteredRecipes(Target, searchFilter);
}

TArray<FInventoryCategory> UMounteaCraftingStatics::GetAllowedCategoriesWithCraftableItems(UObject* Target)
{
	if (!IsValidRecipeHandler(Target))
		return {};

	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings))
		return {};

	const TMap<FString, FInventoryCategory> allowedCategories = settings->GetAllowedCategories();
	if (allowedCategories.Num() == 0)
		return {};

	FMounteaCraftingRecipeSearchFilter searchFilter;
	searchFilter.bSearchByAvailableIngredients = true;

	const TArray<UMounteaRecipeTemplate*> craftableRecipes = GetFilteredRecipes(Target, searchFilter);
	if (craftableRecipes.Num() == 0)
		return {};

	TSet<FString> craftableCategoryIds;
	craftableCategoryIds.Reserve(FMath::Min(craftableRecipes.Num(), allowedCategories.Num()));

	for (const UMounteaRecipeTemplate* recipe : craftableRecipes)
	{
		if (!IsValid(recipe))
			continue;

		const UMounteaInventoryItemTemplate* resultItem = recipe->ResultItem.LoadSynchronous();
		if (!IsValid(resultItem))
			continue;

		const FString& itemCategory = resultItem->ItemCategory;
		if (craftableCategoryIds.Contains(itemCategory))
			continue;

		if (!allowedCategories.Contains(itemCategory))
			continue;

		craftableCategoryIds.Add(itemCategory);
		if (craftableCategoryIds.Num() == allowedCategories.Num())
			break;
	}

	if (craftableCategoryIds.IsEmpty())
		return {};

	TArray<FInventoryCategory> returnValue;
	returnValue.Reserve(allowedCategories.Num());

	Algo::TransformIf(
		allowedCategories,
		returnValue,
		[&craftableCategoryIds](const TPair<FString, FInventoryCategory>& category)
		{
			return craftableCategoryIds.Contains(category.Key);
		},
		[](const TPair<FString, FInventoryCategory>& category)
		{
			return category.Value;
		});

	return returnValue;
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

TScriptInterface<IMounteaAdvancedInventoryInterface> UMounteaCraftingStatics::GetParentInventory(UObject* Target)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_GetParentInventory(Target) : TScriptInterface<IMounteaAdvancedInventoryInterface>();
}

bool UMounteaCraftingStatics::SetParentInventory(UObject* Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_SetParentInventory(Target, NewParentInventory) : false;
}

bool UMounteaCraftingStatics::StartUsingCraftingStation(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_StartUsingCraftingStation(Target, Station) : false;
}

bool UMounteaCraftingStatics::StopUsingCraftingStation(UObject* Target)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_StopUsingCraftingStation(Target) : false;
}

TScriptInterface<IMounteaAdvancedCraftingStationInterface> UMounteaCraftingStatics::GetCraftingStation(UObject* Target)
{
	return IsValidRecipeHandler(Target) ? IMounteaAdvancedCraftingParticipantInterface::Execute_GetCraftingStation(Target) : TScriptInterface<IMounteaAdvancedCraftingStationInterface>();
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

FMounteaCraftingResult UMounteaCraftingStatics::CraftItem(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target, const UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	FMounteaCraftingResult result;

	if (!Target || !IsValid(TemplateToCraft) || !IsValid(Ingredients))
		return result;
	
	const auto craftingStation = Target->Execute_GetCraftingStation(Target.GetObject());
	if (TemplateToCraft->RequiredCraftingPlace.IsValid() && !IsValid(craftingStation.GetObject()))
		return result;
	
	if (IsValid(craftingStation.GetObject()) && TemplateToCraft->RequiredCraftingPlace != craftingStation->Execute_GetCraftingPlaceType(craftingStation.GetObject()))
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

bool UMounteaCraftingStatics::CanBeUsed(UObject* Target)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_CanBeUsed(Target) : false;
}

ECraftingStationState UMounteaCraftingStatics::GetCraftingStationState(UObject* Target)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_GetCraftingStationState(Target) : ECraftingStationState::EASS_Inactive;
}

bool UMounteaCraftingStatics::SetCraftingStationState(UObject* Target, const ECraftingStationState NewState)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_SetCraftingStationState(Target, NewState) : false;
}

bool UMounteaCraftingStatics::StartUsing(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_StartUsing(Target, Participant) : false;
}

bool UMounteaCraftingStatics::StopUsing(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant)
{
	return IsValidCraftingPlace(Target) ? IMounteaAdvancedCraftingStationInterface::Execute_StopUsing(Target, Participant) : false;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingStatics::GetRecipesByCategory(UObject* Target, const FGameplayTag& CategoryTag)
{
	return GetFilteredRecipesByCategory(Target, CategoryTag);
}
