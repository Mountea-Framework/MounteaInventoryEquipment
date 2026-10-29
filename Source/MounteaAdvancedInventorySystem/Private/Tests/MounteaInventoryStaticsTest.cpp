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

#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Statics/MounteaInventoryStatics.h"
#include "UObject/Package.h"

namespace
{
	UMounteaInventoryItemTemplate* MakeTemplate(const int32 MaxQuantity)
	{
		UMounteaInventoryItemTemplate* Template = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
		Template->MaxQuantity = MaxQuantity;
		return Template;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryStaticsNullTemplateTest, "Mountea.InventorySystem.Statics.NullTemplateItemInvalid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryStaticsNullTemplateTest::RunTest(const FString& Parameters)
{
	const FMounteaInventoryItem Item(nullptr, 1);
	TestFalse(TEXT("Item with null template is invalid"), Item.IsItemValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryStaticsClampQuantityTest, "Mountea.InventorySystem.Statics.ItemClampsQuantity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryStaticsClampQuantityTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* Template = MakeTemplate(5);

	TestEqual(TEXT("Quantity above max is clamped"), FMounteaInventoryItem(Template, 99).GetQuantity(), 5);
	TestEqual(TEXT("Quantity below max is kept"), FMounteaInventoryItem(Template, 3).GetQuantity(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryStaticsIsItemValidTest, "Mountea.InventorySystem.Statics.IsInventoryItemValid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryStaticsIsItemValidTest::RunTest(const FString& Parameters)
{
	const FMounteaInventoryItem Valid(MakeTemplate(5), 1);
	const FMounteaInventoryItem Invalid;

	TestTrue(TEXT("Valid item"), UMounteaInventoryStatics::IsInventoryItemValid(Valid));
	TestFalse(TEXT("Default item is invalid"), UMounteaInventoryStatics::IsInventoryItemValid(Invalid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryStaticsGetGuidsTest, "Mountea.InventorySystem.Statics.GetInventoryItemGuidsOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryStaticsGetGuidsTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* Template = MakeTemplate(5);
	const TArray<FMounteaInventoryItem> Items = {
		FMounteaInventoryItem(Template, 1), FMounteaInventoryItem(Template, 2), FMounteaInventoryItem(Template, 3) };

	TArray<FString> Errors;
	const TArray<FGuid> Guids = UMounteaInventoryStatics::GetInventoryItemGuids(Items, Errors);

	if (!TestEqual(TEXT("One guid per valid item"), Guids.Num(), Items.Num()))
		return false;
	for (int32 i = 0; i < Items.Num(); ++i)
		TestEqual(*FString::Printf(TEXT("Guid %d in input order"), i), Guids[i], Items[i].GetGuid());
	return true;
}

#endif
