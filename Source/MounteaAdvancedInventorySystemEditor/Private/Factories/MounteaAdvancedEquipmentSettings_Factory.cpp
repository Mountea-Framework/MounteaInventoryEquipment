// Fill out your copyright notice in the Description page of Project Settings.


#include "MounteaAdvancedEquipmentSettings_Factory.h"

#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedEquipmentSettings_Factory::UMounteaAdvancedEquipmentSettings_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedEquipmentSettingsConfig::StaticClass();
}

UObject* UMounteaAdvancedEquipmentSettings_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedEquipmentSettingsConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedEquipmentSettings_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Settings Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass
	(
		TitleText, ChosenClass, UMounteaAdvancedEquipmentSettingsConfig::StaticClass()
	);

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
