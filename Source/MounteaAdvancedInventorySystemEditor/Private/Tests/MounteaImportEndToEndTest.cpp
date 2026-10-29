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
	// Non-zip, non-.mnteaitems file takes the importer's single-item raw-JSON path.
	const FString path = dir / TEXT("item.json");

	if (UMounteaAdvancedInventoryItemTemplateEditorStatics::FindTemplateByGuid(
			UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates(), itemGuid))
	{
		AddError(TEXT("Generated GUID already exists in project; refusing to import"));
		return true;
	}

	FFileHelper::SaveStringToFile(
		FString::Printf(TEXT("{\"guid\":\"%s\",\"displayName\":\"%s\"}"), *itemGuid.ToString(EGuidFormats::DigitsWithHyphens), *itemName),
		*path);

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

// Non-zip content falls to the legacy JSON path, where ParseMultipleTemplatesJson fails -> returns false, nothing written.
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

#endif
