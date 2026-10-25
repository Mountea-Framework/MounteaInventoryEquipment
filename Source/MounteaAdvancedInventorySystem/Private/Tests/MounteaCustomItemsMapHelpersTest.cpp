// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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

#if WITH_DEV_AUTOMATION_TESTS

#include "Helpers/MounteaCustomItemsMapHelpers.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCustomItemsMapAddRemoveTest, "Mountea.InventorySystem.CustomItemsMap.AddRemove",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCustomItemsMapAddRemoveTest::RunTest(const FString& Parameters)
{
	FMounteaCustomItemsMapHelpers::FCustomItemsMap Map;
	const FGameplayTag Tag; // empty tag is a valid map key; avoids depending on registered tags
	const FGuid A = FGuid::NewGuid();
	const FGuid B = FGuid::NewGuid();

	TestFalse(TEXT("Empty map stores nothing"), FMounteaCustomItemsMapHelpers::IsItemStored(Map, Tag, A));
	TestFalse(TEXT("Remove from empty map fails"), FMounteaCustomItemsMapHelpers::RemoveCustomItem(Map, Tag, A));

	FMounteaCustomItemsMapHelpers::AddCustomItem(Map, Tag, A);
	FMounteaCustomItemsMapHelpers::AddCustomItem(Map, Tag, A);
	FMounteaCustomItemsMapHelpers::AddCustomItem(Map, Tag, B);
	TestTrue(TEXT("A stored"), FMounteaCustomItemsMapHelpers::IsItemStored(Map, Tag, A));
	TestEqual(TEXT("Duplicate Add is ignored"), Map[Tag].StoredIds.Num(), 2);

	TestTrue(TEXT("Remove A"), FMounteaCustomItemsMapHelpers::RemoveCustomItem(Map, Tag, A));
	TestFalse(TEXT("A gone"), FMounteaCustomItemsMapHelpers::IsItemStored(Map, Tag, A));
	TestTrue(TEXT("B kept"), FMounteaCustomItemsMapHelpers::IsItemStored(Map, Tag, B));
	TestFalse(TEXT("Remove A twice fails"), FMounteaCustomItemsMapHelpers::RemoveCustomItem(Map, Tag, A));

	TestTrue(TEXT("Remove B"), FMounteaCustomItemsMapHelpers::RemoveCustomItem(Map, Tag, B));
	TestFalse(TEXT("Empty tag entry is erased"), Map.Contains(Tag));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCustomItemsMapAppendClearTest, "Mountea.InventorySystem.CustomItemsMap.AppendClear",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCustomItemsMapAppendClearTest::RunTest(const FString& Parameters)
{
	FMounteaCustomItemsMapHelpers::FCustomItemsMap Map, Other;
	const FGameplayTag Tag;
	const FGuid A = FGuid::NewGuid();

	FMounteaCustomItemsMapHelpers::AddCustomItem(Other, Tag, A);
	FMounteaCustomItemsMapHelpers::AppendCustomItems(Map, Other);
	TestTrue(TEXT("Appended item is stored"), FMounteaCustomItemsMapHelpers::IsItemStored(Map, Tag, A));

	FMounteaCustomItemsMapHelpers::ClearCustomItems(Map);
	TestEqual(TEXT("Clear empties map"), Map.Num(), 0);
	TestTrue(TEXT("Clear does not touch source"), FMounteaCustomItemsMapHelpers::IsItemStored(Other, Tag, A));
	return true;
}

#endif
