// All rights reserved Dominik Pavlicek 2022.


#include "InventoryItemAssetFactory.h"

#include "AssetToolsModule.h"
#include "Definitions/InventoryItem.h"

UInventoryItemAssetFactory::UInventoryItemAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UInventoryItem::StaticClass();
}

UObject* UInventoryItemAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UInventoryItem* NewItem = NewObject<UInventoryItem>(InParent, Class, Name, Flags, Context);
	
	return NewItem;
}