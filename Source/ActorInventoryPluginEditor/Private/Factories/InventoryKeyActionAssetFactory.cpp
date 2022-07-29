// All rights reserved Dominik Pavlicek 2022.


#include "Factories/InventoryKeyActionAssetFactory.h"

#include "Definitions/InventoryKeyAction.h"

UInventoryKeyActionAssetFactory::UInventoryKeyActionAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UInventoryKeyAction::StaticClass();
}

UObject* UInventoryKeyActionAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UInventoryKeyAction>(InParent, Class, Name, Flags, Context);
}
