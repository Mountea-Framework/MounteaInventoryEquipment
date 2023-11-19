// All rights reserved Dominik Morse 2022.


#include "MounteaThemeAssetFactory.h"

#include "Factories/MounteaItemConfigAssetFactory.h"

#include "Settings/MounteaInventoryThemeConfig.h"

#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"


UMounteaThemeAssetFactory::UMounteaThemeAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryThemeConfig::StaticClass();
}


UObject* UMounteaThemeAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMounteaInventoryThemeConfig* NewTheme = NewObject<UMounteaInventoryThemeConfig>(InParent, ParentClass, Name, Flags, Context);

	if (NewTheme)
	{
		NewTheme->GenerateMissingThemes_Editor();
	}

	return NewTheme;
}


bool UMounteaThemeAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Theme Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInventoryThemeConfig::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}