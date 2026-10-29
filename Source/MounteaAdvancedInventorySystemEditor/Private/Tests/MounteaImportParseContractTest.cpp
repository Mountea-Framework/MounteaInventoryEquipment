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

#include "Helpers/MounteaInventoryZipHelper.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Guid.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	/** Loads Resources/Example/<FileName>; false (with an error) if the plugin or file is missing. */
	bool LoadFixture(FAutomationTestBase& Test, const TCHAR* FileName, TArray<uint8>& OutBytes)
	{
		const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"));
		if (!plugin.IsValid())
		{
			Test.AddError(TEXT("Plugin MounteaAdvancedInventorySystem not found"));
			return false;
		}
		const FString path = plugin->GetBaseDir() / TEXT("Resources/Example") / FileName;
		if (!FMounteaInventoryZipHelper::LoadFileToBytes(path, OutBytes))
		{
			Test.AddError(FString::Printf(TEXT("Fixture missing: %s"), *path));
			return false;
		}
		return true;
	}

	FString JoinKeys(const TMap<FString, FString>& Text, const TMap<FString, TArray<uint8>>& Binary)
	{
		TArray<FString> keys;
		Text.GetKeys(keys);
		Binary.GetKeys(keys);
		return FString::Join(keys, TEXT(", "));
	}

	/**
	 * Loads a bundle fixture, unwraps the first inner entity and parses its primary JSON.
	 * Reports the actual outer/inner layout in the error when anything is absent.
	 */
	TSharedPtr<FJsonObject> LoadFirstInnerJson(FAutomationTestBase& Test, const TCHAR* FileName,
		const TCHAR* InnerExtension, const TCHAR* PrimaryJson)
	{
		TArray<uint8> bytes;
		if (!LoadFixture(Test, FileName, bytes))
			return nullptr;

		TArray<TArray<uint8>> inner;
		if (!FMounteaInventoryZipHelper::ExpandBundle(bytes, InnerExtension, inner) || inner.IsEmpty())
		{
			TMap<FString, FString> text;
			TMap<FString, TArray<uint8>> binary;
			FMounteaInventoryZipHelper::ExtractEntries(bytes, text, binary);
			Test.AddError(FString::Printf(TEXT("%s: no '.%s' entries. Outer entries: [%s]"),
				FileName, InnerExtension, *JoinKeys(text, binary)));
			return nullptr;
		}

		TMap<FString, FString> text;
		TMap<FString, TArray<uint8>> binary;
		if (!FMounteaInventoryZipHelper::ExtractEntries(inner[0], text, binary) || !text.Contains(PrimaryJson))
		{
			Test.AddError(FString::Printf(TEXT("%s: first inner entity lacks '%s'. Inner entries: [%s]"),
				FileName, PrimaryJson, *JoinKeys(text, binary)));
			return nullptr;
		}

		TSharedPtr<FJsonObject> obj;
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text[PrimaryJson]), obj) || !obj.IsValid())
		{
			Test.AddError(FString::Printf(TEXT("%s: '%s' is not a JSON object"), FileName, PrimaryJson));
			return nullptr;
		}
		return obj;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaImportContractItemTest,
	"Mountea.InventorySystemEditor.ImportContract.Item",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaImportContractItemTest::RunTest(const FString& Parameters)
{
	const TSharedPtr<FJsonObject> obj = LoadFirstInnerJson(*this, TEXT("items.mnteaitems"), TEXT("mnteaitem"), TEXT("item.json"));
	if (!obj.IsValid())
		return true;

	FGuid guid;
	TestTrue(TEXT("guid is valid"), FGuid::Parse(obj->GetStringField(TEXT("guid")), guid) && guid.IsValid());
	TestFalse(TEXT("displayName non-empty"), obj->GetStringField(TEXT("displayName")).IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaImportContractRecipeTest,
	"Mountea.InventorySystemEditor.ImportContract.Recipe",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaImportContractRecipeTest::RunTest(const FString& Parameters)
{
	const TSharedPtr<FJsonObject> obj = LoadFirstInnerJson(*this, TEXT("recipes.mntearecipes"), TEXT("mntearecipe"), TEXT("recipe.json"));
	if (!obj.IsValid())
		return true;

	TestTrue(TEXT("root guid"), obj->HasField(TEXT("guid")));
	TestTrue(TEXT("root qtyMax"), obj->HasField(TEXT("qtyMax")));
	TestTrue(TEXT("reqs object"), obj->HasTypedField<EJson::Object>(TEXT("reqs")));
	TestTrue(TEXT("ingredients array"), obj->HasTypedField<EJson::Array>(TEXT("ingredients")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaImportContractLoadoutTest,
	"Mountea.InventorySystemEditor.ImportContract.Loadout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaImportContractLoadoutTest::RunTest(const FString& Parameters)
{
	const TSharedPtr<FJsonObject> obj = LoadFirstInnerJson(*this, TEXT("loadouts.mntealoadouts"), TEXT("mntealoadout"), TEXT("loadout.json"));
	if (!obj.IsValid())
		return true;

	TestTrue(TEXT("root guid"), obj->HasField(TEXT("guid")));
	const TArray<TSharedPtr<FJsonValue>>* items = nullptr;
	if (!TestTrue(TEXT("items array"), obj->TryGetArrayField(TEXT("items"), items)))
		return true;

	for (const TSharedPtr<FJsonValue>& v : *items)
	{
		const TSharedPtr<FJsonObject>* item = nullptr;
		const TSharedPtr<FJsonObject>* ref = nullptr;
		const bool ok = v.IsValid() && v->TryGetObject(item) && (*item)->TryGetObjectField(TEXT("ref"), ref)
			&& (*ref)->HasField(TEXT("guid"));
		TestTrue(TEXT("item has ref.guid"), ok);
	}
	return true;
}

// Workspace is a flat zip: items/<guid>.json, crafting/<guid>.json, loadouts/<guid>.json, taxonomy.json.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaImportContractWorkspaceTest,
	"Mountea.InventorySystemEditor.ImportContract.Workspace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaImportContractWorkspaceTest::RunTest(const FString& Parameters)
{
	TArray<uint8> bytes;
	if (!LoadFixture(*this, TEXT("mountea-workspace.mnteainventory"), bytes))
		return true;

	TMap<FString, FString> text;
	TMap<FString, TArray<uint8>> binary;
	if (!TestTrue(TEXT("Extract"), FMounteaInventoryZipHelper::ExtractEntries(bytes, text, binary)))
		return true;

	const FString layout = FString::Printf(TEXT("Entries: [%s]"), *JoinKeys(text, binary));
	for (const TCHAR* prefix : {TEXT("items/"), TEXT("crafting/"), TEXT("loadouts/")})
	{
		bool found = false;
		for (const TPair<FString, FString>& kv : text)
			found |= kv.Key.StartsWith(prefix);
		TestTrue(FString::Printf(TEXT("Has %s entries. %s"), prefix, *layout), found);
	}
	TestTrue(FString::Printf(TEXT("Has taxonomy.json. %s"), *layout), text.Contains(TEXT("taxonomy.json")));
	return true;
}

#endif
