// All rights reserved Dominik Morse 2022.


#include "MounteaDataTableAssetFactory_Base.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Definitions/MounteaInventoryTableTypes.h"
#include "Helpers/MounteaItemHelpers.h"

UMounteaDataTableAssetFactory_Base::UMounteaDataTableAssetFactory_Base()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemsTable::StaticClass();
	Struct = FMounteaInventoryItemData::StaticStruct();
}

UObject* UMounteaDataTableAssetFactory_Base::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
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
}

UMounteaItemDescriptionsDataTableAssetFactory::UMounteaItemDescriptionsDataTableAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemsDescriptionsTable::StaticClass();
	Struct = FMounteaItemDescription::StaticStruct();
}

UObject* UMounteaItemDescriptionsDataTableAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMounteaInventoryItemsDescriptionsTable* MounteaInventoryItemsDescriptionsTable = nullptr;
	if (Struct && ensure(SupportedClass == Class))
	{
		ensure(0 != (RF_Public & Flags));
		MounteaInventoryItemsDescriptionsTable = NewObject<UMounteaInventoryItemsDescriptionsTable>(InParent, Name, Flags);
		if (MounteaInventoryItemsDescriptionsTable)
		{
			MounteaInventoryItemsDescriptionsTable->RowStruct = const_cast<UScriptStruct*>(ToRawPtr(Struct));
		}
	}

	return MounteaInventoryItemsDescriptionsTable;
}
