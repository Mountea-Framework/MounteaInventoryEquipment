// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventorySettings_Factory.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

UMounteaAdvancedInventorySettings_Factory::UMounteaAdvancedInventorySettings_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventorySettingsConfig::StaticClass();
}

UObject* UMounteaAdvancedInventorySettings_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventorySettingsConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventorySettings_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Settings Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventorySettingsConfig::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}
