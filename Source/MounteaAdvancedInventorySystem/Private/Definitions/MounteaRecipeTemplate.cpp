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


#include "Definitions/MounteaRecipeTemplate.h"

#include "Definitions/MounteaRecipeItem.h"

#if WITH_EDITOR

void UMounteaRecipeTemplate::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaRecipeTemplate, RecipeIngredientOptions))
	{
		if (!(PropertyChangedEvent.ChangeType & (EPropertyChangeType::ArrayAdd | EPropertyChangeType::Duplicate)))
			return;

		const int32 itemIndex = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UMounteaRecipeTemplate, RecipeIngredientOptions));
		int32 resolvedItemIndex = itemIndex;
		if (!RecipeIngredientOptions.IsValidIndex(resolvedItemIndex))
		{
			resolvedItemIndex = RecipeIngredientOptions.IndexOfByPredicate([](const TObjectPtr<UMounteaRecipeItem>& Item)
			{
				return Item == nullptr;
			});
		}

		if (!RecipeIngredientOptions.IsValidIndex(resolvedItemIndex) || RecipeIngredientOptions[resolvedItemIndex] != nullptr)
			return;

		RecipeIngredientOptions[resolvedItemIndex] = NewObject<UMounteaRecipeItem>(this, UMounteaRecipeItem::StaticClass(), NAME_None, RF_Transactional);
		Modify();
	}
}

#endif