// All rights reserved Dominik Pavlicek 2023


#include "MounteaItemAssetFactory.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"

UMounteaItemAssetFactory::UMounteaItemAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItem::StaticClass();
}

UObject* UMounteaItemAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (ParentClass == nullptr)
	{
		ParentClass = UMounteaInventoryItem::StaticClass();
	}
	
	UMounteaInventoryItem* Item = NewObject<UMounteaInventoryItem>(InParent, ParentClass, Name, Flags, Context);
	
	if (SourceTable)
	{
		Item->SourceTable = SourceTable;
		Item->SourceRow = SourceRow;

		Item->SetValidData();
	}

	return Item;
	return NewObject<UMounteaInventoryItem>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaItemAssetFactory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Item"));
	ParentClass = nullptr;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaInventoryEquipmentEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, SupportedClass);
	if (bPressedOk)
	{
		ParentClass = ChosenClass;
	}

	return bPressedOk;
}

void UMounteaItemAssetFactory::SetParentClass(const TSubclassOf<UMounteaInventoryItemsTable>& InParentClass)
{
	ParentClass = InParentClass;
}

void UMounteaItemAssetFactory::SetSource(UMounteaInventoryItemsTable* InSourceTable, const FName& InSourceRow)
{
	SourceTable = InSourceTable;
	SourceRow = InSourceRow;
}
