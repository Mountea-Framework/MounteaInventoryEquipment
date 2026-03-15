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

#include "MounteaRecipeIngredient_Factory.h"

#include "Definitions/MounteaRecipeIngredient.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaRecipeIngredient_Factory::UMounteaRecipeIngredient_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaRecipeIngredient::StaticClass();
}

UObject* UMounteaRecipeIngredient_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaRecipeIngredient>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaRecipeIngredient_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Recipe Ingredient"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(
		TitleText,
		ChosenClass,
		UMounteaRecipeIngredient::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

