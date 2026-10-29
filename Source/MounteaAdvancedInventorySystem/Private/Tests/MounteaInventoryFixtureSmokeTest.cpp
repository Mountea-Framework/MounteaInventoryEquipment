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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryFixtureSpawnInventoryTest, "Mountea.InventorySystem.Fixture.SpawnInventory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaInventoryFixtureSpawnInventoryTest::RunTest(const FString& Parameters)
{
	MounteaInventoryTest::FInventoryWorldFixture fixture;
	if (!fixture.Setup(*this))
	{
		return false;
	}

	AActor* actor = fixture.SpawnActor();
	TestNotNull(TEXT("Actor spawned"), actor);
	if (actor)
	{
		UMounteaInventoryComponent* inventory = fixture.AddComponent<UMounteaInventoryComponent>(actor);
		TestNotNull(TEXT("Inventory component created"), inventory);
		TestTrue(TEXT("Standalone actor has authority"), actor->HasAuthority());

		UMounteaInventoryItemTemplate* itemTemplate = MounteaInventoryTest::FInventoryWorldFixture::MakeTemplate(5);
		TestEqual(TEXT("Template MaxQuantity settable"), itemTemplate->MaxQuantity, 5);
	}

	fixture.Tick(2);
	fixture.Teardown(*this);
	return true;
}

#endif
