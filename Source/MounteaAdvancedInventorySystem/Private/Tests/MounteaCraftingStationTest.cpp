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
#include "Components/MounteaCraftingParticipantComponent.h"
#include "Components/MounteaCraftingStationComponent.h"
#include "Components/MounteaInventoryComponent.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingStationInterface.h"

using MounteaInventoryTest::FInventoryWorldFixture;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStationOccupancyTest, "Mountea.InventorySystem.CraftingStation.Occupancy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStationOccupancyTest::RunTest(const FString& Parameters)
{
	FInventoryWorldFixture fixture;
	if (!fixture.Setup(*this))
	{
		return false;
	}

	UMounteaCraftingStationComponent* station = fixture.AddComponent<UMounteaCraftingStationComponent>(fixture.SpawnActor());

	// Inventory goes first on each actor: participant BeginPlay looks it up.
	AActor* actorA = fixture.SpawnActor();
	fixture.AddComponent<UMounteaInventoryComponent>(actorA);
	UMounteaCraftingParticipantComponent* participantA = fixture.AddComponent<UMounteaCraftingParticipantComponent>(actorA);
	AActor* actorB = fixture.SpawnActor();
	fixture.AddComponent<UMounteaInventoryComponent>(actorB);
	UMounteaCraftingParticipantComponent* participantB = fixture.AddComponent<UMounteaCraftingParticipantComponent>(actorB);
	fixture.Tick(1);

	if (TestNotNull(TEXT("Station created"), station) && TestNotNull(TEXT("Participant A created"), participantA) && TestNotNull(TEXT("Participant B created"), participantB))
	{
		// Station is driven directly: participant-side StartUsingCraftingStation additionally requires known recipes.
		const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> asA(participantA);
		const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> asB(participantB);

		TestTrue(TEXT("Fresh station can be used"), IMounteaAdvancedCraftingStationInterface::Execute_CanBeUsed(station));
		TestTrue(TEXT("First participant starts using"), IMounteaAdvancedCraftingStationInterface::Execute_StartUsing(station, asA));
		TestTrue(TEXT("Station occupied"), IMounteaAdvancedCraftingStationInterface::Execute_IsCraftingPlaceOccupied(station));
		TestFalse(TEXT("Occupied station cannot be used"), IMounteaAdvancedCraftingStationInterface::Execute_CanBeUsed(station));
		TestFalse(TEXT("Second participant rejected"), IMounteaAdvancedCraftingStationInterface::Execute_StartUsing(station, asB));

		TestTrue(TEXT("StopUsing succeeds"), IMounteaAdvancedCraftingStationInterface::Execute_StopUsing(station, asA));
		TestTrue(TEXT("Station can be used again"), IMounteaAdvancedCraftingStationInterface::Execute_CanBeUsed(station));
	}

	fixture.Tick(1);
	fixture.Teardown(*this);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
