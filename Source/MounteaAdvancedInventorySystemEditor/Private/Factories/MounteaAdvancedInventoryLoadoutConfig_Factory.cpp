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

#include "MounteaAdvancedInventoryLoadoutConfig_Factory.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"

#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedInventoryLoadoutConfigs_Factory::UMounteaAdvancedInventoryLoadoutConfigs_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryLoadoutConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryLoadoutConfigs_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryLoadoutConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryLoadoutConfigs_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Inventory & Equipment Loadouts Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryLoadoutConfig::StaticClass());

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
