// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemsTableAssetAction.h"

#include "ToolMenus.h"
#include "Misc/FileHelper.h"
#include "EditorFramework/AssetImportData.h"

#include "Editor/DataTableEditor/Public/DataTableEditorModule.h"
#include "DesktopPlatformModule.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemCategory.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Definitions/MounteaInventoryTableTypes.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "Factories/MounteaItemAssetFactory.h"


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
		AdvancedMenuSubCategoryName_02
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
			DataTablesListText.AppendLineFormat(LOCTEXT("MounteaInventoryTable_MissingRowStructListEntry", "* {0} (Row Structure: {1})"), FText::FromString(Table->GetName()), FText::FromName(ResolvedRowStructName));
		}

		FText Title = LOCTEXT("MounteaInventoryTable_MissingRowStructTitle", "Continue?");
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

void FMounteaInventoryItemsTableAssetAction::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
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
			FExecuteAction::CreateSP( this, &FMounteaInventoryItemsTableAssetAction::ExecuteExportAsJSON, Tables ),
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
			FExecuteAction::CreateSP( this, &FMounteaInventoryItemsTableAssetAction::ProcessFindSourceFileInExplorer, ImportPaths, PotentialFileExtensions ),
				FCanExecuteAction::CreateSP(this, &FMounteaInventoryItemsTableAssetAction::CanProcessFindSourceFileInExplorer, ImportPaths, PotentialFileExtensions)
		);

		Section.AddMenuEntry("MounteaInventoryTable_OpenSourceData", Label, ToolTip, Icon, UIExecuteAction);
	}

	// Generate new Items
	const TAttribute<FText> Label =LOCTEXT("MounteaInventoryTable_GenerateNewItems", "Generate New Items");
	const TAttribute<FText> ToolTip = LOCTEXT("MounteaInventoryTable_GenerateNewItemsTooltip", "For each row in selected Inventory Tables will make a new Item Asset, if such Item doesn't exist yet.");
	const FSlateIcon Icon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Refresh");
	const FUIAction UIExecuteAction = FUIAction
		(
			FExecuteAction::CreateSP( this, &FMounteaInventoryItemsTableAssetAction::GenerateNewItems, Tables ),
			FCanExecuteAction()
		);

	Section.AddMenuEntry("MounteaInventoryTable_GenerateNewItems", Label, ToolTip, Icon, UIExecuteAction);
}

void FMounteaInventoryItemsTableAssetAction::ExecuteExportAsJSON(TArray<TWeakObjectPtr<UObject>> Objects)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto DataTable = Cast<UDataTable>((*ObjIt).Get());
		if (DataTable)
		{
			const FText Title = FText::Format(LOCTEXT("MounteaInventoryTable_ExportJSONDialogTitle", "Export '{0}' as JSON..."), FText::FromString(*DataTable->GetName()));
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

void FMounteaInventoryItemsTableAssetAction::GenerateNewItems(TArray<TWeakObjectPtr<UObject>> Objects)
{
	// Build a package name to use
	//FString MeshName;
	FString PackageName;
	if (PackageName.IsEmpty())
	{
		//FString NewNameSuggestion = FString(TEXT("StaticMesh"));
		FString PackageNameSuggestion = FString(TEXT("/Game/MounteaInventory/Data/")); // + NewNameSuggestion;
		FString Name;
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(PackageNameSuggestion, TEXT(""), PackageNameSuggestion, Name);

		TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
			SNew(SDlgPickAssetPath)
			.Title(LOCTEXT("GenerateNewItemsLocationPicker", "Choose New Items Location"))
			.DefaultAssetPath(FText::FromString(PackageNameSuggestion));

		if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
		{
			PackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
		}
	}
	
	const FString DefaultSuffix = TEXT("_Imported");

	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		const auto Object = (*ObjIt).Get();
		
		if (Object == nullptr)
		{
			return;
		}

		TArray<UObject*> ObjectsToSync;
		
		if ( UMounteaInventoryItemsTable* ItemsTable = Cast<UMounteaInventoryItemsTable>(Object) )
		{
			FString Context;
			TArray<FName> InventoryItemDataNames = ItemsTable->GetRowNames();

			if (InventoryItemDataNames.Num() == 0)
			{
				return;
			}

			for (const auto& Itr : InventoryItemDataNames)
			{
				// Create the factory used to generate the asset
				UMounteaItemAssetFactory* Factory = NewObject<UMounteaItemAssetFactory>();
				Factory->SetParentClass(UMounteaInventoryItemBase::StaticClass());
				Factory->SetSource(ItemsTable, Itr);

				FString FindRowContext;
				const FMounteaInventoryItemData* CurrentItemRow = ItemsTable->FindRow<FMounteaInventoryItemData>(Itr, FindRowContext);
				
				// Determine an appropriate name
				FString Name;
				FString PackagePath;
				FString LocalPackageName = PackageName;

				if (CurrentItemRow && CurrentItemRow->RequiredData.ItemCategory)
				{
					LocalPackageName.Append("/").Append(CurrentItemRow->RequiredData.ItemCategory->CategoryName.ToString()).Append("/");
				}

				// Hacking the suffix to fit the Name
				CreateUniqueAssetName(LocalPackageName, Itr.ToString(), PackagePath, Name);

				FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
				UObject* NewAsset = AssetToolsModule.Get().CreateAsset(Name, FPackageName::GetLongPackagePath(PackagePath), UMounteaInventoryItemBase::StaticClass(), Factory);

				if ( NewAsset )
				{
					ObjectsToSync.Add(NewAsset);
				}
			}

			if ( ObjectsToSync.Num() > 0 )
			{
				FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
				ContentBrowserModule.Get().SyncBrowserToAssets(ObjectsToSync);
			}
		}
	}
}

void FMounteaInventoryItemsTableAssetAction::ProcessFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions)
{
	for (TArray<FString>::TConstIterator FilenameIter(Filenames); FilenameIter; ++FilenameIter)
	{
		const FString CSVFilename = FPaths::ConvertRelativePathToFull(*FilenameIter);
		const FString RootPath = FPaths::GetPath(CSVFilename);
		const FString BaseFilename = FPaths::GetBaseFilename(CSVFilename, true);
		
		for (TArray<FString>::TConstIterator ExtensionItr(OverrideExtensions); ExtensionItr; ++ExtensionItr)
		{
			const FString FilenameWithExtension(FString::Printf(TEXT("%s/%s%s"), *RootPath, *BaseFilename, **ExtensionItr));
			if (CheckFileExists(FilenameWithExtension))
			{
				FPlatformProcess::LaunchFileInDefaultExternalApplication(*FilenameWithExtension, NULL, ELaunchVerb::Edit);
				break;
			}
		}
	}
}

bool FMounteaInventoryItemsTableAssetAction::CanProcessFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions) const
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
			if (CheckFileExists(FilenameWithExtension))
			{
				return true;
			}
		}
	}

	return false;
}

bool FMounteaInventoryItemsTableAssetAction::CheckFileExists(const FString& InFileName) const
{
	return (!InFileName.IsEmpty() && IFileManager::Get().FileSize(*InFileName) != INDEX_NONE);
}

#undef LOCTEXT_NAMESPACE
