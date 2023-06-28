// All rights reserved Dominik Pavlicek 2022.


#include "MounteaItemsDataTableAssetFactory.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Definitions/MounteaInventoryItemsTable.h"


UMounteaItemsDataTableAssetFactory::UMounteaItemsDataTableAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemsTable::StaticClass();
	Struct = FMounteaInventoryItemData::StaticStruct();
}

UObject* UMounteaItemsDataTableAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMounteaInventoryItemsTable* MounteaInventoryItemsTable = nullptr;
	if (Struct && ensure(SupportedClass == Class))
	{
		ensure(0 != (RF_Public & Flags));
		MounteaInventoryItemsTable = NewObject<UMounteaInventoryItemsTable>(InParent, Name, Flags);
		if (MounteaInventoryItemsTable)
		{
			MounteaInventoryItemsTable->RowStruct = const_cast<UScriptStruct*>(ToRawPtr(Struct));
		}
	}

	return MounteaInventoryItemsTable;
	
	// Something is not right!
	if (SupportedClass == nullptr)
	{
		FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a Inventory Assets Table!"));
		return nullptr;
	}
	
	return NewObject<UMounteaInventoryItemsTable>(SupportedClass, Class, Name, Flags, Context);
}

