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
#include "Misc/Paths.h"

namespace
{
	/** Loads Resources/Example/items.mnteaitems; false (with an error) if the plugin or file is missing. */
	bool LoadItemsFixture(FAutomationTestBase& Test, TArray<uint8>& OutBytes)
	{
		const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"));
		if (!plugin.IsValid())
		{
			Test.AddError(TEXT("Plugin MounteaAdvancedInventorySystem not found"));
			return false;
		}
		const FString path = plugin->GetBaseDir() / TEXT("Resources/Example/items.mnteaitems");
		if (!FMounteaInventoryZipHelper::LoadFileToBytes(path, OutBytes))
		{
			Test.AddError(FString::Printf(TEXT("Fixture missing: %s"), *path));
			return false;
		}
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryZipIsZipFileTest,
	"Mountea.InventorySystemEditor.ZipHelper.IsZipFile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryZipIsZipFileTest::RunTest(const FString& Parameters)
{
	TestFalse(TEXT("Empty"), FMounteaInventoryZipHelper::IsZipFile({}));
	TestFalse(TEXT("Non-zip bytes"), FMounteaInventoryZipHelper::IsZipFile({0x7B, 0x7D, 0x00, 0x00}));
	TestTrue(TEXT("PK local-file magic"), FMounteaInventoryZipHelper::IsZipFile({0x50, 0x4B, 0x03, 0x04}));

	TArray<uint8> bytes;
	if (LoadItemsFixture(*this, bytes))
		TestTrue(TEXT("Real fixture"), FMounteaInventoryZipHelper::IsZipFile(bytes));
	return true;
}

// items.mnteaitems is a zip of zips: the outer archive holds only non-JSON (inner zip) entries.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryZipExtractEntriesTest,
	"Mountea.InventorySystemEditor.ZipHelper.ExtractEntriesBundle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryZipExtractEntriesTest::RunTest(const FString& Parameters)
{
	TArray<uint8> bytes;
	if (!LoadItemsFixture(*this, bytes))
		return true;

	TMap<FString, FString> text;
	TMap<FString, TArray<uint8>> binary;
	TestTrue(TEXT("Outer extract"), FMounteaInventoryZipHelper::ExtractEntries(bytes, text, binary));
	TestTrue(TEXT("Has inner entries"), binary.Num() > 0);

	// Unwrap the first inner single-entity zip: it must contain item.json.
	for (const TPair<FString, TArray<uint8>>& kv : binary)
	{
		TMap<FString, FString> innerText;
		TMap<FString, TArray<uint8>> innerBinary;
		TestTrue(TEXT("Inner is zip"), FMounteaInventoryZipHelper::IsZipFile(kv.Value));
		TestTrue(TEXT("Inner extract"), FMounteaInventoryZipHelper::ExtractEntries(kv.Value, innerText, innerBinary));
		TestTrue(TEXT("Inner has item.json"), innerText.Contains(TEXT("item.json")));
		break;
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
