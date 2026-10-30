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

#include "Helpers/MounteaInventoryReimportHelpers.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/Package.h"

namespace
{
	TArray<uint8> ReimportJsonBytes(const FString& Json)
	{
		FTCHARToUTF8 utf8(*Json);
		TArray<uint8> bytes;
		bytes.Append(reinterpret_cast<const uint8*>(utf8.Get()), utf8.Length());
		return bytes;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaReimportSourcesTest,
	"Mountea.InventorySystemEditor.Reimport.SourceRecords",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaReimportSourcesTest::RunTest(const FString& Parameters)
{
	const auto plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"));
	if (!TestTrue(TEXT("Plugin available"), plugin.IsValid())) return false;
	FMounteaInventoryReimportSource source;
	const FString workspace = FPaths::Combine(plugin->GetBaseDir(), TEXT("Resources/Example/mountea-workspace.mnteainventory"));
	if (!TestTrue(TEXT("Workspace parses"), FMounteaInventoryReimportHelpers::ReadSource(workspace, source))) return false;
	TestEqual(TEXT("Workspace contains all item records"), source.Items.Num(), 13);
	TestEqual(TEXT("Workspace crafting folder contains all recipe records"), source.Recipes.Num(), 6);
	TestEqual(TEXT("Each recipe has station lookup"), source.RecipeStations.Num(), 6);
	const FString directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation/ReimportSources_") + FGuid::NewGuid().ToString(EGuidFormats::Digits));
	IFileManager::Get().MakeDirectory(*directory, true);
	const FGuid rootGuid = FGuid::NewGuid();
	const FGuid dependencyGuid = FGuid::NewGuid();
	const FString rootJson = FString::Printf(TEXT("{\"guid\":\"%s\"}"), *rootGuid.ToString(EGuidFormats::DigitsWithHyphens));
	const FString dependencyJson = FString::Printf(TEXT("{\"guid\":\"%s\"}"), *dependencyGuid.ToString(EGuidFormats::DigitsWithHyphens));
	for (const FString& type : TArray<FString>{TEXT("item"), TEXT("recipe"), TEXT("loadout")})
	{
		const FString extension = TEXT("mntea") + type;
		TArray<uint8> bytes;
		FString error;
		TestTrue(TEXT("Build entity fixture"), FMounteaInventoryZipHelper::BuildEntityArchive(type + TEXT(".json"), rootJson, {dependencyJson}, bytes, error));
		const TMap<FGuid, TArray<uint8>> archives = {{rootGuid, bytes}};
		for (const FString& suffix : TArray<FString>{TEXT(""), TEXT("s")})
		{
			const FString path = FPaths::Combine(directory, TEXT("source.") + extension + suffix);
			TestTrue(TEXT("Write canonical entity source"), FMounteaInventoryZipHelper::WriteEntityArchives(archives, path, extension, extension + TEXT("s"), error));
			TestTrue(TEXT("Read single or bundle entity source"), FMounteaInventoryReimportHelpers::ReadSource(path, source));
			TestTrue(TEXT("Source-backed item dependency is addressable by its own GUID"), source.Items.Contains(dependencyGuid));
			TestEqual(TEXT("Only item and recipe roots exposed"), source.Items.Contains(rootGuid), type == TEXT("item"));
			TestEqual(TEXT("Only recipe roots exposed as recipes"), source.Recipes.Contains(rootGuid), type == TEXT("recipe"));
		}
	}
	TMap<FString, TArray<uint8>> invalidEntries;
	invalidEntries.Add(TEXT("items/") + rootGuid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json"), ReimportJsonBytes(dependencyJson));
	TArray<uint8> invalidArchive;
	TestTrue(TEXT("Build mismatching GUID path source"), FMounteaInventoryZipHelper::BuildArchive(invalidEntries, invalidArchive));
	const FString invalidPath = FPaths::Combine(directory, TEXT("invalid.mnteainventory"));
	TestTrue(TEXT("Write mismatching GUID path source"), FFileHelper::SaveArrayToFile(invalidArchive, *invalidPath));
	TestFalse(TEXT("Workspace mismatched filename GUID rejected"), FMounteaInventoryReimportHelpers::ReadSource(invalidPath, source));
	TestTrue(TEXT("GUID mismatch explains failure"), !source.Error.IsEmpty());
	IFileManager::Get().DeleteDirectory(*directory, false, true);
	TestFalse(TEXT("Unsupported source rejected"), FMounteaInventoryReimportHelpers::ReadSource(TEXT("source.json"), source));
	TestTrue(TEXT("Unsupported source explains failure"), !source.Error.IsEmpty());
	TestFalse(TEXT("Missing source rejected"), FMounteaInventoryReimportHelpers::ReadSource(TEXT("missing.mnteaitem"), source));
	TestTrue(TEXT("Missing source explains failure"), !source.Error.IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaReimportBatchTest,
	"Mountea.InventorySystemEditor.Reimport.ExistingAssetsOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaReimportBatchTest::RunTest(const FString& Parameters)
{
	const FString suffix = FGuid::NewGuid().ToString(EGuidFormats::Digits);
	const FString folder = TEXT("/Game/__MounteaReimportTest_") + suffix;
	const FString directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation/Reimport_") + suffix);
	IFileManager::Get().MakeDirectory(*directory, true);
	const FString sourcePath = FPaths::Combine(directory, TEXT("workspace.mnteainventory"));
	const FGuid itemGuid = FGuid::NewGuid();
	const FGuid recipeGuid = FGuid::NewGuid();
	const FString itemJson = FString::Printf(TEXT("{\"guid\":\"%s\",\"displayName\":\"Reimported item\"}"), *itemGuid.ToString(EGuidFormats::DigitsWithHyphens));
	const FString recipeJson = FString::Printf(TEXT("{\"guid\":\"%s\",\"qtyMax\":3,\"result\":{\"itemRef\":\"%s\"},\"ingredients\":[{\"ref\":{\"guid\":\"%s\"},\"qty\":1}]}"),
		*recipeGuid.ToString(EGuidFormats::DigitsWithHyphens), *itemGuid.ToString(EGuidFormats::DigitsWithHyphens), *itemGuid.ToString(EGuidFormats::DigitsWithHyphens));
	TMap<FString, TArray<uint8>> entries;
	entries.Add(TEXT("items/") + itemGuid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json"), ReimportJsonBytes(itemJson));
	entries.Add(TEXT("crafting/") + recipeGuid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json"), ReimportJsonBytes(recipeJson));
	TArray<uint8> archive;
	TestTrue(TEXT("Build workspace fixture"), FMounteaInventoryZipHelper::BuildArchive(entries, archive));
	TestTrue(TEXT("Write workspace fixture"), FFileHelper::SaveArrayToFile(archive, *sourcePath));
	UPackage* itemPackage = CreatePackage(*(folder + TEXT("/Item")));
	auto* item = NewObject<UMounteaInventoryItemTemplate>(itemPackage, TEXT("Item"), RF_Public | RF_Standalone);
	item->Guid = itemGuid;
	item->DisplayName = FText::FromString(TEXT("Before"));
	item->SetSourceFilePath(sourcePath);
	FAssetRegistryModule::AssetCreated(item);
	UPackage* recipePackage = CreatePackage(*(folder + TEXT("/Recipe")));
	auto* recipe = NewObject<UMounteaRecipeTemplate>(recipePackage, TEXT("Recipe"), RF_Public | RF_Standalone);
	recipe->RecipeGuid = recipeGuid;
	recipe->QuantityPerCreation = 9;
	recipe->SetSourceFilePath(sourcePath);
	FAssetRegistryModule::AssetCreated(recipe);
	auto* missing = NewObject<UMounteaInventoryItemTemplate>(itemPackage, TEXT("MissingGuid"), RF_Public | RF_Standalone);
	missing->Guid = FGuid::NewGuid();
	missing->SetSourceFilePath(sourcePath);
	auto* noSource = NewObject<UMounteaInventoryItemTemplate>(itemPackage, TEXT("NoSource"), RF_Public | RF_Standalone);
	auto* draft = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage(), NAME_None, RF_Transient);
	draft->SetSourceFilePath(sourcePath);
	itemPackage->SetDirtyFlag(false);
	recipePackage->SetDirtyFlag(false);
	const auto itemResult = FMounteaInventoryReimportHelpers::Reimport({item, missing, noSource, draft});
	TestEqual(TEXT("Only matching persistent item updated"), itemResult.Updated.Num(), 1);
	TestEqual(TEXT("Missing GUID is a per-asset failure"), itemResult.Failures.Num(), 1);
	TestEqual(TEXT("Unbacked asset skipped"), itemResult.Skipped, 1);
	TestEqual(TEXT("Item state updated in place"), item->DisplayName.ToString(), FString(TEXT("Reimported item")));
	TestEqual(TEXT("Item reimport leaves recipe unchanged"), recipe->QuantityPerCreation, 9);
	const auto recipeResult = FMounteaInventoryReimportHelpers::Reimport({recipe});
	TestEqual(TEXT("Existing recipe updated"), recipeResult.Updated.Num(), 1);
	TestEqual(TEXT("Recipe state updated"), recipe->QuantityPerCreation, 3);
	TestEqual(TEXT("Recipe resolves existing item"), recipe->ResultItem.Get(), item);
	TestEqual(TEXT("Recorded path retained"), item->GetSourceFilePath(), sourcePath);
	TestTrue(TEXT("Saved item exists"), FPaths::FileExists(FPackageName::LongPackageNameToFilename(itemPackage->GetName(), FPackageName::GetAssetPackageExtension())));
	FAssetRegistryModule::AssetDeleted(recipe);
	FAssetRegistryModule::AssetDeleted(item);
	for (UObject* asset : TArray<UObject*>{item, recipe, missing, noSource})
	{
		asset->ClearFlags(RF_Public | RF_Standalone);
		asset->MarkAsGarbage();
	}
	itemPackage->SetDirtyFlag(false);
	recipePackage->SetDirtyFlag(false);
	IFileManager::Get().DeleteDirectory(*FPackageName::LongPackageNameToFilename(folder), false, true);
	IFileManager::Get().DeleteDirectory(*directory, false, true);
	return true;
}

#endif
