// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "MounteaAdvancedEquipmentUIConfig_Factory.h"

#include "Settings/MounteaAdvancedEquipmentUIConfig.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedEquipmentUIConfig_Factory::UMounteaAdvancedEquipmentUIConfig_Factory()
{
	this->bCreateNew = true;
	this->bEditAfterNew = true;

	this->SupportedClass = UMounteaAdvancedEquipmentUIConfig::StaticClass();
}

UObject* UMounteaAdvancedEquipmentUIConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedEquipmentUIConfig>(InParent, this->ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedEquipmentUIConfig_Factory::ConfigureProperties()
{
	static const FText titleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Equipment UI Config"));

	UClass* chosenClass = nullptr;
	const bool pressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(titleText, chosenClass, UMounteaAdvancedEquipmentUIConfig::StaticClass());

	if (pressedOk)
		this->ParentClass = chosenClass;

	return pressedOk;
}
