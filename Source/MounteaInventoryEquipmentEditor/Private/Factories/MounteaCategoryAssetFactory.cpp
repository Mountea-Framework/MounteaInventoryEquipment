// All rights reserved Dominik Morse 2022.


#include "MounteaCategoryAssetFactory.h"

#include "Definitions/MounteaInventoryItemCategory.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"


UMounteaCategoryAssetFactory::UMounteaCategoryAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemCategory::StaticClass();
}


UObject* UMounteaCategoryAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInventoryItemCategory>(InParent, ParentClass, Name, Flags, Context);
}


bool UMounteaCategoryAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Category Type"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInventoryItemCategory::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}