// All rights reserved Dominik Pavlicek 2022.


#include "Factories/InventoryCategoryAssetFactory.h"

#include "Definitions/InventoryCategory.h"

UInventoryCategoryAssetFactory::UInventoryCategoryAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UInventoryCategory::StaticClass();
}

UObject* UInventoryCategoryAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UInventoryCategory>(InParent, Class, Name, Flags, Context);
}
