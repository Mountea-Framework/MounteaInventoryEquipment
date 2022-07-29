// All rights reserved Dominik Pavlicek 2022.


#include "Factories/InventoryRarityAssetFactory.h"

#include "Definitions/InventoryItemRarity.h"

UInventoryRarityAssetFactory::UInventoryRarityAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UInventoryItemRarity::StaticClass();
}

UObject* UInventoryRarityAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UInventoryItemRarity>(InParent, Class, Name, Flags, Context);
}
