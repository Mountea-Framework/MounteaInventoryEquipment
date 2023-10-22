// All rights reserved Dominik Pavlicek 2022.


#include "MounteaContentThemeAssetFactory.h"

#include "Settings/ContentTheme.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"


UMounteaContentThemeAssetFactory::UMounteaContentThemeAssetFactory()
{
	bCreateNew = false;
	bEditAfterNew = true;

	SupportedClass = UContentTheme::StaticClass();
}


UObject* UMounteaContentThemeAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UContentTheme* NewTheme = NewObject<UContentTheme>(InParent, ParentClass, Name, Flags, Context);

	return NewTheme;
}


bool UMounteaContentThemeAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Content Theme"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UContentTheme::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

UTextThemeAssetFactory::UTextThemeAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UTextTheme::StaticClass();
}

UObject* UTextThemeAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTextTheme* NewTheme = NewObject<UTextTheme>(InParent, ParentClass, Name, Flags, Context);

	return NewTheme;
}

bool UTextThemeAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Text Theme"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UTextTheme::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

UCategoryThemeAssetFactory::UCategoryThemeAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UCategoryTheme::StaticClass();
}

UObject* UCategoryThemeAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UCategoryTheme* NewTheme = NewObject<UCategoryTheme>(InParent, ParentClass, Name, Flags, Context);

	return NewTheme;
}

bool UCategoryThemeAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Category Theme"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UCategoryTheme::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

USlotThemeAssetFactory::USlotThemeAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = USlotTheme::StaticClass();
}

UObject* USlotThemeAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	USlotTheme* NewTheme = NewObject<USlotTheme>(InParent, ParentClass, Name, Flags, Context);

	return NewTheme;
}

bool USlotThemeAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Slot Theme"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, USlotTheme::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}
