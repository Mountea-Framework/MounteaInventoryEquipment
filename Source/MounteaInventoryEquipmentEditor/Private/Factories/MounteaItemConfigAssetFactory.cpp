// All rights reserved Dominik Pavlicek 2022.


#include "Factories/MounteaItemConfigAssetFactory.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "Setup/MounteaInventoryItemConfig.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"


UMounteaItemConfigAssetFactory::UMounteaItemConfigAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemConfig::StaticClass();
}


UObject* UMounteaItemConfigAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInventoryItemConfig>(InParent, Class, Name, Flags, Context);
}


bool UMounteaItemConfigAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent  Class for new Mountea Inventory Item Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInventoryItemConfig::StaticClass());

	if ( bPressedOk )
	{
		SupportedClass = ChosenClass;
	}

	return bPressedOk;
}