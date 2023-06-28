// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryTableAssetAction_Base.h"

#include "ContentBrowserMenuContexts.h"
#include "DataTableEditorModule.h"
#include "Definitions/MounteaInventoryTableTypes.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemsTableAssetAction"

FMounteaInventoryTableAssetAction_Base::FMounteaInventoryTableAssetAction_Base()
{}

FText FMounteaInventoryTableAssetAction_Base::GetName() const
{
	return LOCTEXT("MounteaInventoryItemsTableAssetAction_Name", "Items Data Table");
}

FColor FMounteaInventoryTableAssetAction_Base::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaInventoryTableAssetAction_Base::GetSupportedClass() const
{
	return UMounteaInventoryItemsTable::StaticClass();
}

uint32 FMounteaInventoryTableAssetAction_Base::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryTableAssetAction_Base::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_02
	};
	return AssetTypeActionSubMenu;
}

void FMounteaInventoryTableAssetAction_Base::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
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
