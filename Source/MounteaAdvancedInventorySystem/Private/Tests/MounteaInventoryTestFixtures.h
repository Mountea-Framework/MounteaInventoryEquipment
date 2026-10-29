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

#pragma once

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR // editor-only: needs an editor world, kept out of game builds

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/Package.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

// Standalone net mode: every spawned actor has authority, so server-side logic is testable without PIE.
namespace MounteaInventoryTest
{
	struct FInventoryWorldFixture
	{
		FTestWorldWrapper WorldWrapper;

		UWorld* GetWorld() const { return WorldWrapper.GetTestWorld(); }

		bool Setup(FAutomationTestBase& Test)
		{
						
			if (!WorldWrapper.CreateTestWorld(EWorldType::Game) || !WorldWrapper.GetTestWorld())
			{
				Test.AddError(TEXT("Failed to create test world"));
				return false;
			}
			WorldWrapper.BeginPlayInTestWorld();
			return true;
		}

		void Teardown(FAutomationTestBase& Test)
		{
			WorldWrapper.ForwardErrorMessages(&Test);
			WorldWrapper.DestroyTestWorld(true);
		}

		void Tick(const int32 Count = 1)
		{
			for (int32 i = 0; i < Count; ++i)
			{
				WorldWrapper.TickTestWorld(0.05f);
			}
		}

		AActor* SpawnActor() const
		{
			UWorld* world = GetWorld();
			return world ? world->SpawnActor<AActor>() : nullptr;
		}

		template<typename T>
		T* AddComponent(AActor* Owner) const
		{
			if (!Owner) return nullptr;
			T* component = NewObject<T>(Owner);
			Owner->AddInstanceComponent(component);
			component->RegisterComponent();
			return component;
		}

		static UMounteaInventoryItemTemplate* MakeTemplate(const int32 MaxQuantity)
		{
			UMounteaInventoryItemTemplate* itemTemplate = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
			itemTemplate->MaxQuantity = MaxQuantity;
			return itemTemplate;
		}

		static FMounteaInventoryItem MakeItem(UMounteaInventoryItemTemplate* ItemTemplate, const int32 Quantity)
		{
			return FMounteaInventoryItem(ItemTemplate, Quantity);
		}
	};
}

#endif
