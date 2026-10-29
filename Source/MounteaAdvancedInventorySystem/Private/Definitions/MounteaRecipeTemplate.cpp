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
#include "UObject/UnrealType.h"


void UMounteaRecipeTemplate::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	if (DuplicateMode == EDuplicateMode::Normal)
		RecipeGuid = FGuid::NewGuid();
}

#if WITH_EDITOR

void UMounteaRecipeTemplate::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName propertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (propertyName != GET_MEMBER_NAME_CHECKED(UMounteaRecipeTemplate, RecipeIngredients)
		|| !(PropertyChangedEvent.ChangeType & (EPropertyChangeType::ArrayAdd | EPropertyChangeType::Duplicate)))
		return;

	int32 ingredientIndex = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UMounteaRecipeTemplate, RecipeIngredients));
	if (!RecipeIngredients.IsValidIndex(ingredientIndex))
	{
		ingredientIndex = RecipeIngredients.IndexOfByPredicate([](const TObjectPtr<UMounteaRecipeIngredient>& Ingredient)
		{
			return Ingredient == nullptr;
		});
	}
	if (!RecipeIngredients.IsValidIndex(ingredientIndex) || RecipeIngredients[ingredientIndex] != nullptr)
		return;

	UClass* ingredientClass = DefaultIngredientClass.Get();
	if (!IsValid(ingredientClass) || ingredientClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
		ingredientClass = UMounteaRecipeIngredient::StaticClass();

	Modify();
	RecipeIngredients[ingredientIndex] = NewObject<UMounteaRecipeIngredient>(this, ingredientClass, NAME_None, RF_Transactional);
}

FString UMounteaRecipeTemplate::GetSourceFilePath() const
{
	return SourceFilePath;
}

void UMounteaRecipeTemplate::SetSourceFilePath(const FString& InPath)
{
	SourceFilePath = InPath;
}

#endif
