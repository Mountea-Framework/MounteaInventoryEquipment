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

#include "Helpers/MounteaInventoryImportHelpers.h"

// Regression: a colon or space in a JSON name once silently broke recipe import.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventorySanitizeAssetNameTest,
	"Mountea.InventorySystemEditor.ImportHelpers.SanitizeAssetName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventorySanitizeAssetNameTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Colon and space"), FMounteaInventoryImportHelpers::SanitizeAssetName(TEXT("Iron Sword: Mk II")), FString(TEXT("IronSwordMkII")));
	TestEqual(TEXT("All illegal chars"), FMounteaInventoryImportHelpers::SanitizeAssetName(TEXT("a\\:*?\"<>|,.&!~@#'b")), FString(TEXT("ab")));
	TestEqual(TEXT("Clean name untouched"), FMounteaInventoryImportHelpers::SanitizeAssetName(TEXT("Potion_01")), FString(TEXT("Potion_01")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryExtractGuidTest,
	"Mountea.InventorySystemEditor.ImportHelpers.ExtractGuidFromJsonField",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryExtractGuidTest::RunTest(const FString& Parameters)
{
	const FGuid expected(0x08d7b9af, 0x11112222, 0x33334444, 0x55556666);
	const FString json = FString::Printf(TEXT("{\"guid\":\"%s\"}"), *expected.ToString(EGuidFormats::DigitsWithHyphens));

	TestEqual(TEXT("Valid field"), FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(json, TEXT("guid")), expected);
	TestFalse(TEXT("Missing field"), FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(json, TEXT("other")).IsValid());
	TestFalse(TEXT("Malformed JSON"), FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(TEXT("{not json"), TEXT("guid")).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryParseBundledGuidsTest,
	"Mountea.InventorySystemEditor.ImportHelpers.ParseBundledGuids",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryParseBundledGuidsTest::RunTest(const FString& Parameters)
{
	const FGuid a(1, 2, 3, 4);
	const FGuid b(5, 6, 7, 8);
	const TArray<FString> jsons = {
		FString::Printf(TEXT("{\"guid\":\"%s\"}"), *a.ToString(EGuidFormats::DigitsWithHyphens)),
		FString::Printf(TEXT("{\"guid\":\"%s\"}"), *b.ToString(EGuidFormats::DigitsWithHyphens)),
		TEXT("{\"guid\":\"garbage\"}")
	};

	const TSet<FGuid> guids = FMounteaInventoryImportHelpers::ParseBundledGuids(jsons);
	TestEqual(TEXT("Invalid entries skipped"), guids.Num(), 2);
	TestTrue(TEXT("Contains a and b"), guids.Contains(a) && guids.Contains(b));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
