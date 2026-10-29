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

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "MounteaInventoryTestFixtures.h"
#include "Components/MounteaInventoryComponent.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"

namespace MounteaInventoryTest
{
	// Spawns a world + actor + inventory, runs Body(inventory), tears down.
	template<typename TBody>
	bool RunWithInventory(FAutomationTestBase& Test, TBody Body)
	{
		FInventoryWorldFixture fixture;
		if (!fixture.Setup(Test))
		{
			return false;
		}

		AActor* actor = fixture.SpawnActor();
		UMounteaInventoryComponent* inventory = fixture.AddComponent<UMounteaInventoryComponent>(actor);
		if (Test.TestNotNull(TEXT("Inventory component created"), inventory))
		{
			Body(inventory);
		}

		fixture.Tick(1);
		fixture.Teardown(Test);
		return true;
	}
}

using MounteaInventoryTest::FInventoryWorldFixture;
using MounteaInventoryTest::RunWithInventory;

#define MOUNTEA_INVENTORY_TEST(ClassName, Name) \
	IMPLEMENT_SIMPLE_AUTOMATION_TEST(ClassName, "Mountea.InventorySystem.Inventory." Name, \
		EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

MOUNTEA_INVENTORY_TEST(FMounteaInventoryAddItemTest, "AddItem")
bool FMounteaInventoryAddItemTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 3);
		TestTrue(TEXT("AddItem succeeds"), IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, item));

		const TArray<FMounteaInventoryItem> all = IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv);
		TestEqual(TEXT("One item stored"), all.Num(), 1);
		if (all.Num() == 1)
		{
			TestEqual(TEXT("Quantity stored"), all[0].GetQuantity(), 3);
		}
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryStackAddTest, "StackAdd")
bool FMounteaInventoryStackAddTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* itemTemplate = FInventoryWorldFixture::MakeTemplate(10);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(itemTemplate, 3));
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(itemTemplate, 4));

		const TArray<FMounteaInventoryItem> all = IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv);
		TestEqual(TEXT("Same template shares one slot"), all.Num(), 1);
		if (all.Num() == 1)
		{
			TestEqual(TEXT("Quantities summed"), all[0].GetQuantity(), 7);
		}
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryPartialRemoveTest, "PartialRemove")
bool FMounteaInventoryPartialRemoveTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 5);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, item);

		TestTrue(TEXT("Decrease succeeds"), IMounteaAdvancedInventoryInterface::Execute_DecreaseItemQuantity(Inv, item.GetGuid(), 2));
		const FMounteaInventoryItem found = IMounteaAdvancedInventoryInterface::Execute_FindItem(Inv, FInventoryItemSearchParams(item.GetGuid()));
		TestEqual(TEXT("Remainder kept"), found.GetQuantity(), 3);
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryFullRemoveTest, "FullRemove")
bool FMounteaInventoryFullRemoveTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 5);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, item);

		TestTrue(TEXT("RemoveItem succeeds"), IMounteaAdvancedInventoryInterface::Execute_RemoveItem(Inv, item.GetGuid()));
		TestEqual(TEXT("Inventory empty"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 0);
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryFindHasTest, "FindHas")
bool FMounteaInventoryFindHasTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 1);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, item);

		TestTrue(TEXT("HasItem finds added"), IMounteaAdvancedInventoryInterface::Execute_HasItem(Inv, FInventoryItemSearchParams(item.GetGuid())));
		TestTrue(TEXT("FindItem returns valid"), IMounteaAdvancedInventoryInterface::Execute_FindItem(Inv, FInventoryItemSearchParams(item.GetGuid())).IsItemValid());
		TestFalse(TEXT("HasItem misses unknown"), IMounteaAdvancedInventoryInterface::Execute_HasItem(Inv, FInventoryItemSearchParams(FGuid::NewGuid())));
		TestFalse(TEXT("FindItem invalid for unknown"), IMounteaAdvancedInventoryInterface::Execute_FindItem(Inv, FInventoryItemSearchParams(FGuid::NewGuid())).IsItemValid());
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryClearTest, "Clear")
bool FMounteaInventoryClearTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 1));
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 1));
		TestEqual(TEXT("Two distinct templates stored"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 2);

		IMounteaAdvancedInventoryInterface::Execute_ClearInventory(Inv);
		TestEqual(TEXT("Cleared"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 0);
	});
}

// ---- Edge cases ----

MOUNTEA_INVENTORY_TEST(FMounteaInventoryOverRemoveTest, "OverRemoveClampsToZero")
bool FMounteaInventoryOverRemoveTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(FInventoryWorldFixture::MakeTemplate(10), 3);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, item);

		// Rule: NewQuantity <= 0 removes the item entirely (never negative).
		TestTrue(TEXT("Over-decrease succeeds"), IMounteaAdvancedInventoryInterface::Execute_DecreaseItemQuantity(Inv, item.GetGuid(), 99));
		TestEqual(TEXT("Item removed"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 0);
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryMaxQuantityTest, "AddPastMaxQuantity")
bool FMounteaInventoryMaxQuantityTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* itemTemplate = FInventoryWorldFixture::MakeTemplate(5);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(itemTemplate, 3));
		// Rule: stacking adds only min(requested, MaxQuantity - held).
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(itemTemplate, 4));

		const TArray<FMounteaInventoryItem> all = IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv);
		TestEqual(TEXT("Still one slot"), all.Num(), 1);
		if (all.Num() == 1)
		{
			TestEqual(TEXT("Capped at MaxQuantity"), all[0].GetQuantity(), 5);
		}
	});
}

MOUNTEA_INVENTORY_TEST(FMounteaInventoryDurabilityClampTest, "DurabilityClamp")
bool FMounteaInventoryDurabilityClampTest::RunTest(const FString& Parameters)
{
	return RunWithInventory(*this, [this](UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* itemTemplate = FInventoryWorldFixture::MakeTemplate(10);
		itemTemplate->bHasDurability = true;
		itemTemplate->MaxDurability = 1.f;
		const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(itemTemplate, 1);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, item);

		// Rule: ModifyItemDurability clamps old+delta to 0..1.
		IMounteaAdvancedInventoryInterface::Execute_ModifyItemDurability(Inv, item.GetGuid(), -5.f);
		TestEqual(TEXT("Clamped to 0"),
			IMounteaAdvancedInventoryInterface::Execute_FindItem(Inv, FInventoryItemSearchParams(item.GetGuid())).GetDurability(), 0.f);

		IMounteaAdvancedInventoryInterface::Execute_ModifyItemDurability(Inv, item.GetGuid(), 5.f);
		TestEqual(TEXT("Clamped to max"),
			IMounteaAdvancedInventoryInterface::Execute_FindItem(Inv, FInventoryItemSearchParams(item.GetGuid())).GetDurability(), 1.f);
	});
}

#undef MOUNTEA_INVENTORY_TEST

#endif
