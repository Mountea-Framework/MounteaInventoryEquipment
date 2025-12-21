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


#include "MounteaAdvancedInventoryUIConfig_Factory.h"

#include "Settings/MounteaAdvancedInventoryUIConfig.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedInventoryUIConfig_Factory::UMounteaAdvancedInventoryUIConfig_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryUIConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryUIConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryUIConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryUIConfig_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory UI Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryUIConfig::StaticClass());

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
