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

#include "MounteaAdvancedInventoryCraftingConfig_Factory.h"

#include "Settings/MounteaAdvancedCraftingConfig.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedInventoryCraftingConfig_Factory::UMounteaAdvancedInventoryCraftingConfig_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedCraftingConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryCraftingConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedCraftingConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryCraftingConfig_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Crafting Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedCraftingConfig::StaticClass());

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
