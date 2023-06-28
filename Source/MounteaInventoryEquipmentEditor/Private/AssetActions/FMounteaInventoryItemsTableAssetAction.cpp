// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemsTableAssetAction.h"

#include "ContentBrowserMenuContexts.h"
#include "DataTableEditorModule.h"
#include "Definitions/MounteaInventoryItemsTable.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemsTableAssetAction"

FMounteaInventoryItemsTableAssetAction::FMounteaInventoryItemsTableAssetAction()
{}

FText FMounteaInventoryItemsTableAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemsTableAssetAction_Name", "Items Data Table");
}

FColor FMounteaInventoryItemsTableAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaInventoryItemsTableAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemsTable::StaticClass();
}

uint32 FMounteaInventoryItemsTableAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryItemsTableAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		LOCTEXT("MounteaInventoryEquipment_Sub03", "3. Data")
	};
	return AssetTypeActionSubMenu;
}

void FMounteaInventoryItemsTableAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	TArray<UDataTable*> DataTablesToOpen;
	TArray<UDataTable*> InvalidDataTables;

	for (UObject* Obj : InObjects)
	{
		UDataTable* Table = Cast<UDataTable>(Obj);
		if (Table)
		{
			if (Table->GetRowStruct())
			{
				DataTablesToOpen.Add(Table);
			}
			else
			{
				InvalidDataTables.Add(Table);
			}
		}
	}

	if (InvalidDataTables.Num() > 0)
	{
		FTextBuilder DataTablesListText;
		DataTablesListText.Indent();
		for (UDataTable* Table : InvalidDataTables)
		{
			const FName ResolvedRowStructName = Table->GetRowStructName();
			DataTablesListText.AppendLineFormat(LOCTEXT("DataTable_MissingRowStructListEntry", "* {0} (Row Structure: {1})"), FText::FromString(Table->GetName()), FText::FromName(ResolvedRowStructName));
		}

		FText Title = LOCTEXT("DataTable_MissingRowStructTitle", "Continue?");
		const EAppReturnType::Type DlgResult = FMessageDialog::Open(
			EAppMsgType::YesNoCancel, 
			FText::Format(LOCTEXT("DataTable_MissingRowStructMsg", "The following Data Tables are missing their row structure and will not be editable.\n\n{0}\n\nDo you want to open these data tables?"), DataTablesListText.ToText()), 
			&Title
			);

		switch(DlgResult)
		{
		case EAppReturnType::Yes:
			DataTablesToOpen.Append(InvalidDataTables);
			break;
		case EAppReturnType::Cancel:
			return;
		default:
			break;
		}
	}

	FDataTableEditorModule& DataTableEditorModule = FModuleManager::LoadModuleChecked<FDataTableEditorModule>("DataTableEditor");
	for (UDataTable* Table : DataTablesToOpen)
	{
		DataTableEditorModule.CreateDataTableEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Table);
	}
}

#undef LOCTEXT_NAMESPACE
