// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "ObjectTools.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Helpers/MounteaInventoryZipHelper.h"

namespace
{
	const FString GTestRoot = TEXT("/Game/MounteaInventoryTests/Import/");

	/** Fresh per-run import folder, e.g. /Game/MounteaInventoryTests/Import/<guid>. */
	FString MakeTestFolder()
	{
		return GTestRoot + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	}

	TArray<FAssetData> GetAssetsUnder(const FString& Folder)
	{
		TArray<FAssetData> assets;
		// Newly written packages may not be indexed yet; rescan the folder so the query sees them.
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get().ScanPathsSynchronous({ Folder }, true);
			FAssetRegistryModule::GetRegistry().GetAssetsByPath(*Folder, assets, true);
		return assets;
	}

	/** Deletes only the given test folder; refuses anything outside GTestRoot or the bare root itself. */
	void CleanupFolder(FAutomationTestBase& Test, const FString& Folder)
	{
		if (!Folder.StartsWith(GTestRoot) || Folder.Len() <= GTestRoot.Len())
		{
			Test.AddError(FString::Printf(TEXT("Refusing to delete unsafe folder: %s"), *Folder));
			return;
		}
		TArray<FAssetData> assets = GetAssetsUnder(Folder);
		if (assets.Num() > 0)
			ObjectTools::DeleteAssets(assets, false);
		IFileManager::Get().DeleteDirectory(*FPackageName::LongPackageNameToFilename(Folder), false, true);
	}

	FString GetFixturePath(FAutomationTestBase& Test, const TCHAR* FileName)
	{
		const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"));
		if (!plugin.IsValid())
		{
			Test.AddError(TEXT("Plugin MounteaAdvancedInventorySystem not found"));
			return FString();
		}
		const FString path = plugin->GetBaseDir() / TEXT("Resources/Example") / FileName;
		if (!FPaths::FileExists(path))
		{
			Test.AddError(FString::Printf(TEXT("Fixture missing: %s"), *path));
			return FString();
		}
		return path;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaImportE2EItemsTest,
	"Mountea.InventorySystemEditor.ImportEndToEnd.ItemsCreateAndReimport",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaImportE2EItemsTest::RunTest(const FString& Parameters)
{
	// Generated item with a fresh GUID: never import shipped fixtures, they match shipped assets by GUID and overwrite them.
	const FGuid itemGuid = FGuid::NewGuid();
	const FString itemName = FString::Printf(TEXT("TestItem%s"), *itemGuid.ToString(EGuidFormats::Digits));
	const FString dir = FPaths::ProjectSavedDir() / TEXT("Tests") / TEXT("MounteaInventoryTests") / itemGuid.ToString(EGuidFormats::Digits);
	// A single item uses the same ZIP transport as recipes and loadouts.
	const FString path = dir / TEXT("item.mnteaitem");

	if (UMounteaAdvancedInventoryItemTemplateEditorStatics::FindTemplateByGuid(
			UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates(), itemGuid))
	{
		AddError(TEXT("Generated GUID already exists in project; refusing to import"));
		return true;
	}

	const FString json = FString::Printf(TEXT("{\"guid\":\"%s\",\"displayName\":\"%s\"}"), *itemGuid.ToString(EGuidFormats::DigitsWithHyphens), *itemName);
	TArray<uint8> archive;
	FString archiveError;
	if (!TestTrue(TEXT("Build canonical item ZIP"), FMounteaInventoryZipHelper::BuildEntityArchive(TEXT("item.json"), json, {}, archive, archiveError))) return false;
	IFileManager::Get().MakeDirectory(*dir, true);
	if (!TestTrue(TEXT("Save canonical item ZIP"), FFileHelper::SaveArrayToFile(archive, *path))) return false;


	const FString folder = MakeTestFolder();

	TArray<UMounteaInventoryItemTemplate*> first;
	FString error;
	TestTrue(TEXT("First import succeeds"),
		UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFilePath(path, folder, first, error));
	TestTrue(TEXT("Import produced templates"), first.Num() > 0);
					bool allInFolder = first.Num() > 0;
		for (const UMounteaInventoryItemTemplate* created : first)
			allInFolder &= IsValid(created) && created->GetOutermost()->GetName().StartsWith(folder);
		TestTrue(TEXT("Templates created under test folder"), allInFolder);

	// Re-import matches by GUID and updates in place, so the asset count must not grow.
	TArray<UMounteaInventoryItemTemplate*> second;
	FString error2;
	UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFilePath(path, folder, second, error2);
	TestEqual(TEXT("Re-import returns the same number of templates"), second.Num(), first.Num());

	IFileManager::Get().DeleteDirectory(*dir, false, true);
	CleanupFolder(*this, folder);
	return true;
}

// Non-ZIP content is rejected without creating assets.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaImportE2ECorruptTest,
	"Mountea.InventorySystemEditor.ImportEndToEnd.CorruptFile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaImportE2ECorruptTest::RunTest(const FString& Parameters)
{
	const FString folder = MakeTestFolder();
	const FString badFile = FPaths::ProjectSavedDir() / TEXT("MounteaInventoryTests") / FGuid::NewGuid().ToString(EGuidFormats::Digits) / TEXT("corrupt.mnteaitems");
	FFileHelper::SaveStringToFile(TEXT("this is definitely not a zip or valid json"), *badFile);

	TArray<UMounteaInventoryItemTemplate*> out;
	FString error;
	TestFalse(TEXT("Corrupt file reports failure"),
		UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFilePath(badFile, folder, out, error));
	TestEqual(TEXT("No templates returned"), out.Num(), 0);
	TestEqual(TEXT("No assets created"), GetAssetsUnder(folder).Num(), 0);

	IFileManager::Get().DeleteDirectory(*FPaths::GetPath(badFile), false, true);
	CleanupFolder(*this, folder);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaItemExportRoundTripTest,
    "Mountea.InventorySystemEditor.ImportEndToEnd.ItemsExportRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FMounteaItemExportRoundTripTest::RunTest(const FString& Parameters)
{
    using FItems = UMounteaAdvancedInventoryItemTemplateEditorStatics;
    auto* first = NewObject<UMounteaInventoryItemTemplate>();
    auto* second = NewObject<UMounteaInventoryItemTemplate>();
    first->Guid = FGuid::NewGuid(); second->Guid = FGuid::NewGuid();
    first->DisplayName = second->DisplayName = FText::FromString(TEXT("Shared item export label"));
    const auto existing = FItems::LoadAllExistingTemplates();
    if (FItems::FindTemplateByGuid(existing, first->Guid) || FItems::FindTemplateByGuid(existing, second->Guid))
    { AddError(TEXT("Fresh test GUID collides with an existing item")); return false; }
    const FString folder = MakeTestFolder();
    const FString directory = FPaths::ProjectSavedDir() / TEXT("Automation/ItemExport") / FGuid::NewGuid().ToString(EGuidFormats::Digits);
    IFileManager::Get().MakeDirectory(*directory, true);
    const FString single = directory / TEXT("single.mnteaitem");
    const FString bundle = directory / TEXT("bundle.mnteaitems");
    FString error;
    TArray<TArray<uint8>> archives;
    if (TestTrue(TEXT("Export canonical single item"), FItems::ExportTemplatesToFilePath({first}, single, error)) &&
        TestTrue(TEXT("Read canonical single item"), FMounteaInventoryZipHelper::ReadEntityArchives(single, TEXT("mnteaitem"), TEXT("mnteaitems"), archives, error)))
    {
        TestEqual(TEXT("One item archive"), archives.Num(), 1);
        FString json;
        if (archives.Num() == 1 && TestTrue(TEXT("Extract item JSON"), FItems::ParseItemJsonFromZip(archives[0], json, error)))
        {
            auto* restored = NewObject<UMounteaInventoryItemTemplate>();
            TestTrue(TEXT("Deserialize exported item"), FItems::ParseSingleTemplateJson(json, restored, error));
            TestEqual(TEXT("Item identity retained"), restored->Guid, first->Guid);
            TestEqual(TEXT("Item display name retained"), restored->DisplayName.ToString(), first->DisplayName.ToString());
        }
    }
    if (TestTrue(TEXT("Export canonical item bundle"), FItems::ExportTemplatesToFilePath({first, second}, bundle, error)))
    {
        TestFalse(TEXT("Duplicate primary item GUID selection rejected"), FItems::ExportTemplatesToFilePath({first, first}, bundle, error));
        TestTrue(TEXT("Failed export leaves bundle readable"), FMounteaInventoryZipHelper::ReadEntityArchives(bundle, TEXT("mnteaitem"), TEXT("mnteaitems"), archives, error));
        TestEqual(TEXT("Both original item archives preserved"), archives.Num(), 2);
        TArray<UMounteaInventoryItemTemplate*> imported {first};
        if (TestTrue(TEXT("Import exported items"), FItems::ImportTemplatesFromFilePath(bundle, folder, imported, error)))
        {
            TestEqual(TEXT("Import outputs contain only this operation"), imported.Num(), 2);
            TestEqual(TEXT("Same display names create two assets"), GetAssetsUnder(folder).Num(), 2);
            if (imported.Num() == 2) TestTrue(TEXT("Unique packages prevent display-name overwrite"), imported[0]->GetOutermost() != imported[1]->GetOutermost());
            TArray<UMounteaInventoryItemTemplate*> reimported;
            TestTrue(TEXT("Reimport exported item bundle"), FItems::ImportTemplatesFromFilePath(bundle, folder, reimported, error));
            TestEqual(TEXT("Reimport creates no duplicates"), GetAssetsUnder(folder).Num(), 2);
            for (const auto* item : reimported) TestTrue(TEXT("Reimport reuses the existing object"), imported.Contains(item));
        }
    }
    CleanupFolder(*this, folder);
    IFileManager::Get().Delete(*single); IFileManager::Get().Delete(*bundle);
    IFileManager::Get().DeleteDirectory(*directory);
    return true;
}

#endif
