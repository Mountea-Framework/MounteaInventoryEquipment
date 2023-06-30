// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryTableAssetAction_Base.h"

#include "ToolMenus.h"
#include "Misc/FileHelper.h"
#include "EditorFramework/AssetImportData.h"

#include "Editor/DataTableEditor/Public/DataTableEditorModule.h"
#include "DesktopPlatformModule.h"
#include "AssetToolsModule.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Definitions/MounteaInventoryTableTypes.h"


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
			DataTablesListText.AppendLineFormat(LOCTEXT("MounteaInventoryTable_MissingRowStructListEntry", "* {0} (Row Structure: {1})"), FText::FromString(Table->GetName()), FText::FromName(ResolvedRowStructName));
		}

		FText Title = LOCTEXT("DataTable_MissingRowStructTitle", "Continue?");
		const EAppReturnType::Type DlgResult = FMessageDialog::Open(
			EAppMsgType::YesNoCancel, 
			FText::Format(LOCTEXT("MounteaInventoryTable_MissingRowStructMsg", "The following Data Tables are missing their row structure and will not be editable.\n\n{0}\n\nDo you want to open these data tables?"), DataTablesListText.ToText()), 
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

void FMounteaInventoryTableAssetAction_Base::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	auto Tables = GetTypedWeakObjectPtrs<UObject>(InObjects);
	
	TArray<FString> ImportPaths;
	for (auto TableIter = Tables.CreateConstIterator(); TableIter; ++TableIter)
	{
		const UDataTable* CurTable = Cast<UDataTable>((*TableIter).Get());
		if (CurTable)
		{
			CurTable->AssetImportData->ExtractFilenames(ImportPaths);
		}
	}

	// Add ExportAsJSON
	{
		const TAttribute<FText> Label = LOCTEXT("MounteaInventoryTable_ExportAsJSON", "Export as JSON");
		const TAttribute<FText> ToolTip = LOCTEXT("MounteaInventoryTable_ExportAsJSONTooltip", "Export the Inventory Table as a file containing JSON data.");
		const FSlateIcon Icon = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset.Small");
		const FUIAction UIExecuteAction = FUIAction
		(
			FExecuteAction::CreateSP( this, &FMounteaInventoryTableAssetAction_Base::ExecuteExportAsJSON, Tables ),
			FCanExecuteAction()
		);

		Section.AddMenuEntry("MounteaInventoryTable_ExportAsJSON", Label, ToolTip, Icon, UIExecuteAction);
	}

	TArray<FString> PotentialFileExtensions;
	PotentialFileExtensions.Add(TEXT(".xls"));
	PotentialFileExtensions.Add(TEXT(".xlsm"));
	PotentialFileExtensions.Add(TEXT(".csv"));
	PotentialFileExtensions.Add(TEXT(".json"));

	// Add OpenSourceData
	{
		const TAttribute<FText> Label =LOCTEXT("MounteaInventoryTable_OpenSourceData", "Open Source Data");
		const TAttribute<FText> ToolTip = LOCTEXT("MounteaInventoryTable_OpenSourceDataTooltip", "Opens the data table's source data file in an external editor. It will search using the following extensions: .xls/.xlsm/.csv/.json");
		const FSlateIcon Icon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Symbols.SearchGlass");
		const FUIAction UIExecuteAction = FUIAction
		(
			FExecuteAction::CreateSP( this, &FMounteaInventoryTableAssetAction_Base::ExecuteFindSourceFileInExplorer, ImportPaths, PotentialFileExtensions ),
				FCanExecuteAction::CreateSP(this, &FMounteaInventoryTableAssetAction_Base::CanExecuteFindSourceFileInExplorer, ImportPaths, PotentialFileExtensions)
		);

		Section.AddMenuEntry("MounteaInventoryTable_OpenSourceData", Label, ToolTip, Icon, UIExecuteAction);
	}
}

void FMounteaInventoryTableAssetAction_Base::ExecuteExportAsJSON(TArray<TWeakObjectPtr<UObject>> Objects)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto DataTable = Cast<UDataTable>((*ObjIt).Get());
		if (DataTable)
		{
			const FText Title = FText::Format(LOCTEXT("DataTable_ExportJSONDialogTitle", "Export '{0}' as JSON..."), FText::FromString(*DataTable->GetName()));
			const FString CurrentFilename = DataTable->AssetImportData->GetFirstFilename();
			const FString FileTypes = TEXT("Data Table JSON (*.json)|*.json");

			TArray<FString> OutFilenames;
			DesktopPlatform->SaveFileDialog(
				ParentWindowWindowHandle,
				Title.ToString(),
				(CurrentFilename.IsEmpty()) ? TEXT("") : FPaths::GetPath(CurrentFilename),
				(CurrentFilename.IsEmpty()) ? TEXT("") : FPaths::GetBaseFilename(CurrentFilename) + TEXT(".json"),
				FileTypes,
				EFileDialogFlags::None,
				OutFilenames
				);

			if (OutFilenames.Num() > 0)
			{
				FFileHelper::SaveStringToFile(DataTable->GetTableAsJSON(EDataTableExportFlags::UseJsonObjectsForStructs), *OutFilenames[0]);
			}
		}
	}
}

void FMounteaInventoryTableAssetAction_Base::ExecuteFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions)
{
	for (TArray<FString>::TConstIterator FilenameIter(Filenames); FilenameIter; ++FilenameIter)
	{
		const FString CSVFilename = FPaths::ConvertRelativePathToFull(*FilenameIter);
		const FString RootPath = FPaths::GetPath(CSVFilename);
		const FString BaseFilename = FPaths::GetBaseFilename(CSVFilename, true);
		
		for (TArray<FString>::TConstIterator ExtensionItr(OverrideExtensions); ExtensionItr; ++ExtensionItr)
		{
			const FString FilenameWithExtension(FString::Printf(TEXT("%s/%s%s"), *RootPath, *BaseFilename, **ExtensionItr));
			if (VerifyFileExists(FilenameWithExtension))
			{
				FPlatformProcess::LaunchFileInDefaultExternalApplication(*FilenameWithExtension, NULL, ELaunchVerb::Edit);
				break;
			}
		}
	}
}

bool FMounteaInventoryTableAssetAction_Base::CanExecuteFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions) const
{
	// Verify that extensions were provided
	if (OverrideExtensions.Num() == 0)
	{
		return false;
	}

	// Verify that the file exists with any of the given extensions
	for (TArray<FString>::TConstIterator FilenameIter(Filenames); FilenameIter; ++FilenameIter)
	{
		const FString CSVFilename = FPaths::ConvertRelativePathToFull(*FilenameIter);
		const FString RootPath = FPaths::GetPath(CSVFilename);
		const FString BaseFilename = FPaths::GetBaseFilename(CSVFilename, true);

		for (TArray<FString>::TConstIterator ExtensionItr(OverrideExtensions); ExtensionItr; ++ExtensionItr)
		{
			const FString FilenameWithExtension(FString::Printf(TEXT("%s/%s%s"), *RootPath, *BaseFilename, **ExtensionItr));
			if (VerifyFileExists(FilenameWithExtension))
			{
				return true;
			}
		}
	}

	return false;
}

bool FMounteaInventoryTableAssetAction_Base::VerifyFileExists(const FString& InFileName) const
{
	return (!InFileName.IsEmpty() && IFileManager::Get().FileSize(*InFileName) != INDEX_NONE);
}

#undef LOCTEXT_NAMESPACE
