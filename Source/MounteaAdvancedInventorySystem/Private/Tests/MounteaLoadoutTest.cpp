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
#include "Components/MounteaAdvancedInventoryLoadoutComponent.h"
#include "Components/MounteaEquipmentComponent.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Loadouts/MounteaAdvancedInventoryLoadoutsInterface.h"
#include "Statics/MounteaLoadoutStatics.h"

namespace MounteaInventoryTest
{
	static UMounteaAdvancedInventoryLoadoutItem* MakeLoadoutItem(UMounteaInventoryItemTemplate* ItemTemplate, const int32 Quantity)
	{
		UMounteaAdvancedInventoryLoadoutItem* item = NewObject<UMounteaAdvancedInventoryLoadoutItem>(GetTransientPackage());
		item->ItemTemplate = ItemTemplate;
		item->BaseQuantity = Quantity;
		item->bAutomaticallyEquip = false;
		return item;
	}

	static int32 TotalQuantity(UObject* Inventory)
	{
		int32 total = 0;
		for (const FMounteaInventoryItem& item : IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inventory))
		{
			total += item.GetQuantity();
		}
		return total;
	}

	// Spawns world + actor + inventory + loadout component (config assigned, autoload off), runs Body(loadout, inventory), tears down.
	template<typename TBody>
	bool RunWithLoadout(FAutomationTestBase& Test, UMounteaAdvancedInventoryLoadoutConfig* Config, TBody Body)
	{
		FInventoryWorldFixture fixture;
		if (!fixture.Setup(Test))
		{
			return false;
		}

		AActor* actor = fixture.SpawnActor();
		UMounteaInventoryComponent* inventory = fixture.AddComponent<UMounteaInventoryComponent>(actor);
		// These loadouts grant items only (no equipment); the UI parent logs this warning by design.
		Test.AddExpectedError(TEXT("Cannot find 'Equipment' component in Parent"), EAutomationExpectedErrorFlags::Contains, 0);
		UMounteaAdvancedInventoryLoadoutComponent* loadout = fixture.AddComponent<UMounteaAdvancedInventoryLoadoutComponent>(actor);
		if (Test.TestNotNull(TEXT("Inventory component created"), inventory) && Test.TestNotNull(TEXT("Loadout component created"), loadout))
		{
			// LoadoutConfiguration is protected with no setter, so assign via reflection.
			const FProperty* configProperty = UMounteaAdvancedInventoryLoadoutComponent::StaticClass()->FindPropertyByName(TEXT("LoadoutConfiguration"));
			if (Test.TestNotNull(TEXT("LoadoutConfiguration property found"), configProperty))
			{
				*configProperty->ContainerPtrToValuePtr<TSoftObjectPtr<UMounteaAdvancedInventoryLoadoutConfig>>(loadout) = Config;
				IMounteaAdvancedInventoryLoadoutsInterface::Execute_SetAutomaticLoad(loadout, false);
				fixture.Tick(1); // BeginPlay resolves the inventory

				Body(loadout, inventory);
			}
		}

		fixture.Tick(1);
		fixture.Teardown(Test);
		return true;
	}

	static UMounteaAdvancedInventoryLoadoutConfig* MakeConfig(const TArray<UMounteaAdvancedInventoryLoadoutItem*>& Items)
	{
		UMounteaAdvancedInventoryLoadoutConfig* config = NewObject<UMounteaAdvancedInventoryLoadoutConfig>(GetTransientPackage());
		for (UMounteaAdvancedInventoryLoadoutItem* item : Items)
		{
			config->Items.Add(item);
		}
		return config;
	}
}

using MounteaInventoryTest::FInventoryWorldFixture;
using MounteaInventoryTest::MakeConfig;
using MounteaInventoryTest::MakeLoadoutItem;
using MounteaInventoryTest::RunWithLoadout;
using MounteaInventoryTest::TotalQuantity;

#define MOUNTEA_LOADOUT_TEST(ClassName, Name) \
	IMPLEMENT_SIMPLE_AUTOMATION_TEST(ClassName, "Mountea.InventorySystem.Loadout." Name, \
		EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

MOUNTEA_LOADOUT_TEST(FMounteaLoadoutGrantsItemTest, "GrantsItem")
bool FMounteaLoadoutGrantsItemTest::RunTest(const FString& Parameters)
{
	UMounteaAdvancedInventoryLoadoutConfig* config = MakeConfig({ MakeLoadoutItem(FInventoryWorldFixture::MakeTemplate(10), 2) });
	return RunWithLoadout(*this, config, [this](UMounteaAdvancedInventoryLoadoutComponent* Loadout, UMounteaInventoryComponent* Inv)
	{
		TestTrue(TEXT("LoadLoadout succeeds"), IMounteaAdvancedInventoryLoadoutsInterface::Execute_LoadLoadout(Loadout));
		TestEqual(TEXT("One item granted"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 1);
		TestEqual(TEXT("Quantity granted"), TotalQuantity(Inv), 2);
	});
}

MOUNTEA_LOADOUT_TEST(FMounteaLoadoutNullTemplateSkippedTest, "NullTemplateSkipped")
bool FMounteaLoadoutNullTemplateSkippedTest::RunTest(const FString& Parameters)
{
	UMounteaAdvancedInventoryLoadoutConfig* config = MakeConfig({
		MakeLoadoutItem(nullptr, 5),
		MakeLoadoutItem(FInventoryWorldFixture::MakeTemplate(10), 3)
	});
	return RunWithLoadout(*this, config, [this](UMounteaAdvancedInventoryLoadoutComponent* Loadout, UMounteaInventoryComponent* Inv)
	{
		// Return value (false: a failed entry) is not asserted; the rule under test is that valid entries still apply.
		IMounteaAdvancedInventoryLoadoutsInterface::Execute_LoadLoadout(Loadout);
		TestEqual(TEXT("Only the valid item granted"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 1);
		TestEqual(TEXT("Valid item quantity"), TotalQuantity(Inv), 3);
	});
}

// LoadLoadout has no "already loaded" guard: a second call goes through AddItemFromTemplate again and stacks.
MOUNTEA_LOADOUT_TEST(FMounteaLoadoutTwiceStacksTest, "LoadTwiceStacks")
bool FMounteaLoadoutTwiceStacksTest::RunTest(const FString& Parameters)
{
	UMounteaAdvancedInventoryLoadoutConfig* config = MakeConfig({ MakeLoadoutItem(FInventoryWorldFixture::MakeTemplate(10), 2) });
	return RunWithLoadout(*this, config, [this](UMounteaAdvancedInventoryLoadoutComponent* Loadout, UMounteaInventoryComponent* Inv)
	{
		IMounteaAdvancedInventoryLoadoutsInterface::Execute_LoadLoadout(Loadout);
		IMounteaAdvancedInventoryLoadoutsInterface::Execute_LoadLoadout(Loadout);
		TestEqual(TEXT("Same template still one slot"), IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv).Num(), 1);
		TestEqual(TEXT("Second load stacks onto the first"), TotalQuantity(Inv), 4);
	});
}

// IsValidLoadout checks the target implements the loadouts interface, so the config asset itself is not valid.
MOUNTEA_LOADOUT_TEST(FMounteaLoadoutIsValidTest, "IsValidLoadout")
bool FMounteaLoadoutIsValidTest::RunTest(const FString& Parameters)
{
	TestFalse(TEXT("Null is not a valid loadout"), UMounteaLoadoutStatics::IsValidLoadout(nullptr));
	TestFalse(TEXT("Config asset does not implement the interface"),
		UMounteaLoadoutStatics::IsValidLoadout(MakeConfig({ MakeLoadoutItem(FInventoryWorldFixture::MakeTemplate(10), 1) })));
	TestTrue(TEXT("Loadout component is valid"),
		UMounteaLoadoutStatics::IsValidLoadout(NewObject<UMounteaAdvancedInventoryLoadoutComponent>(GetTransientPackage())));
	return true;
}

#endif
