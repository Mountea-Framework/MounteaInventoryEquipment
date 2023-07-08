// All rights reserved Dominik Pavlicek 2022.


#include "MounteaInventoryItemAdditionalDataFactory.h"

#include "Definitions/MounteaItemAdditionalData.h"
#include "Settings/MounteaInventoryThemeConfig.h"

#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"


UMounteaInventoryItemAdditionalDataFactory::UMounteaInventoryItemAdditionalDataFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaItemAdditionalData::StaticClass();
}


UObject* UMounteaInventoryItemAdditionalDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaItemAdditionalData>(InParent, ParentClass, Name, Flags, Context);
}


bool UMounteaInventoryItemAdditionalDataFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Item Additional Data"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaItemAdditionalData::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}