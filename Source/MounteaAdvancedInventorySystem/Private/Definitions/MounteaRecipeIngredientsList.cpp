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


#include "Definitions/MounteaRecipeIngredientsList.h"

#include "Definitions/MounteaRecipeIngredient.h"

#if WITH_EDITOR

void UMounteaRecipeIngredientsList::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaRecipeIngredientsList, RecipeIngredients))
	{
		if (!(PropertyChangedEvent.ChangeType & (EPropertyChangeType::ArrayAdd | EPropertyChangeType::Duplicate)))
			return;

		const int32 itemIndex = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UMounteaRecipeIngredientsList, RecipeIngredients));
		int32 resolvedItemIndex = itemIndex;
		if (!RecipeIngredients.IsValidIndex(resolvedItemIndex))
		{
			resolvedItemIndex = RecipeIngredients.IndexOfByPredicate([](const TObjectPtr<UMounteaRecipeIngredient>& Item)
			{
				return Item == nullptr;
			});
		}

		if (!RecipeIngredients.IsValidIndex(resolvedItemIndex) || RecipeIngredients[resolvedItemIndex] != nullptr)
			return;

		RecipeIngredients[resolvedItemIndex] = NewObject<UMounteaRecipeIngredient>(this, UMounteaRecipeIngredient::StaticClass(), NAME_None, RF_Transactional);
		Modify();
	}
}

#endif