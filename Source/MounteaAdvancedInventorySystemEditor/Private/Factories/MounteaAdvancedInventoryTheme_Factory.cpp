// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventoryTheme_Factory.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

UMounteaAdvancedInventoryTheme_Factory::UMounteaAdvancedInventoryTheme_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryThemeConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryTheme_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryThemeConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryTheme_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Theme Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryThemeConfig::StaticClass());

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
