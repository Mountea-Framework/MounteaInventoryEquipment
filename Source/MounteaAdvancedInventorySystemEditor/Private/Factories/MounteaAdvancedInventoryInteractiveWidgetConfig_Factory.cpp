// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventoryInteractiveWidgetConfig_Factory.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"
#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"

UMounteaAdvancedInventoryInteractiveWidgetConfig_Factory::UMounteaAdvancedInventoryInteractiveWidgetConfig_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryInteractiveWidgetConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryInteractiveWidgetConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryInteractiveWidgetConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryInteractiveWidgetConfig_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Interactive Widget Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryInteractiveWidgetConfig::StaticClass());

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}
