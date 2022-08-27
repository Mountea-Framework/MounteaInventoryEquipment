// All rights reserved Dominik Pavlicek 2022.


#include "Factories/InventoryTypeDefinitionAssetFactory.h"

#include "Definitions/InventoryTypeDefinition.h"

UInventoryTypeDefinitionAssetFactory::UInventoryTypeDefinitionAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UInventoryTypeDefinition::StaticClass();
}

UObject* UInventoryTypeDefinitionAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UInventoryTypeDefinition>(InParent, Class, Name, Flags, Context);
}
