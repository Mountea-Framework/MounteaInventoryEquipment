// All rights reserved Dominik Pavlicek 2022.


#include "MounteaRarityAssetFactory.h"

#include "Definitions/MounteaInventoryItemRarity.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"


UMounteaRarityAssetFactory::UMounteaRarityAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemRarity::StaticClass();
}


UObject* UMounteaRarityAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInventoryItemRarity>(InParent, ParentClass, Name, Flags, Context);
}


bool UMounteaRarityAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Rarity Type"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaInventoryItemRarity::StaticClass());

	if ( bPressedOk )
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}