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

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaRecipeIngredientsList.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Statics/MounteaCraftingStatics.h"
#include "UObject/Package.h"

namespace
{
	// The tests pre-seed the quantity cache, so the (empty) inventory interface is never queried.
	using FQuantityCache = TMap<UMounteaInventoryItemTemplate*, int32>;

	UMounteaInventoryItemTemplate* MakeItem()
	{
		return NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
	}

	UMounteaRecipeIngredient* MakeIngredient(UMounteaInventoryItemTemplate* Source, const int32 Required)
	{
		UMounteaRecipeIngredient* Ingredient = NewObject<UMounteaRecipeIngredient>(GetTransientPackage());
		Ingredient->IngredientSource = Source;
		Ingredient->RequiredQuantity = Required;
		return Ingredient;
	}

	UMounteaRecipeIngredientsList* MakeGroup(const TArray<UMounteaRecipeIngredient*>& Ingredients)
	{
		UMounteaRecipeIngredientsList* Group = NewObject<UMounteaRecipeIngredientsList>(GetTransientPackage());
		for (UMounteaRecipeIngredient* Ingredient : Ingredients)
			Group->RecipeIngredients.Add(Ingredient);
		return Group;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStaticsIngredientSatisfiedTest, "Mountea.InventorySystem.CraftingStatics.IsIngredientSatisfied",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStaticsIngredientSatisfiedTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* Item = MakeItem();
	const UMounteaRecipeIngredient* Ingredient = MakeIngredient(Item, 3);
	const TScriptInterface<IMounteaAdvancedInventoryInterface> NoInventory;

	FQuantityCache Cache;
	Cache.Add(Item, 3);
	TestTrue(TEXT("Exact quantity satisfies"), UMounteaCraftingStatics::IsIngredientSatisfied(NoInventory, Ingredient, Cache));

	Cache.Add(Item, 10);
	TestTrue(TEXT("Surplus satisfies"), UMounteaCraftingStatics::IsIngredientSatisfied(NoInventory, Ingredient, Cache));

	Cache.Add(Item, 2);
	TestFalse(TEXT("Shortage does not satisfy"), UMounteaCraftingStatics::IsIngredientSatisfied(NoInventory, Ingredient, Cache));

	TestFalse(TEXT("Null ingredient does not satisfy"), UMounteaCraftingStatics::IsIngredientSatisfied(NoInventory, nullptr, Cache));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStaticsGroupSatisfiedTest, "Mountea.InventorySystem.CraftingStatics.IsIngredientGroupSatisfied",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStaticsGroupSatisfiedTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* ItemA = MakeItem();
	UMounteaInventoryItemTemplate* ItemB = MakeItem();
	const UMounteaRecipeIngredientsList* Group = MakeGroup({ MakeIngredient(ItemA, 2), MakeIngredient(ItemB, 1) });
	const TScriptInterface<IMounteaAdvancedInventoryInterface> NoInventory;

	FQuantityCache Cache;
	Cache.Add(ItemA, 2);
	Cache.Add(ItemB, 1);
	TestTrue(TEXT("Group satisfied when every ingredient is satisfied"), UMounteaCraftingStatics::IsIngredientGroupSatisfied(NoInventory, Group, Cache));

	Cache.Add(ItemB, 0);
	TestFalse(TEXT("Group not satisfied when one ingredient is short"), UMounteaCraftingStatics::IsIngredientGroupSatisfied(NoInventory, Group, Cache));

	TestFalse(TEXT("Null group is not satisfied"), UMounteaCraftingStatics::IsIngredientGroupSatisfied(NoInventory, nullptr, Cache));
	TestTrue(TEXT("Empty group is vacuously satisfied"), UMounteaCraftingStatics::IsIngredientGroupSatisfied(NoInventory, MakeGroup({}), Cache));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStaticsRecipeSatisfiedTest, "Mountea.InventorySystem.CraftingStatics.IsRecipeSatisfiedByIngredients",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStaticsRecipeSatisfiedTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* ItemA = MakeItem();
	UMounteaInventoryItemTemplate* ItemB = MakeItem();
	UMounteaRecipeTemplate* Recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
	const TScriptInterface<IMounteaAdvancedInventoryInterface> NoInventory;

	FQuantityCache Cache;
	TestTrue(TEXT("Recipe without ingredient options is satisfied"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(NoInventory, Recipe, Cache));

	Recipe->RecipeIngredientOptions.Add(MakeGroup({ MakeIngredient(ItemA, 1), MakeIngredient(ItemB, 1) }));

	Cache.Add(ItemA, 1);
	Cache.Add(ItemB, 1);
	TestTrue(TEXT("All ingredients present"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(NoInventory, Recipe, Cache));

	Cache.Add(ItemB, 0);
	TestFalse(TEXT("One ingredient missing"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(NoInventory, Recipe, Cache));

	Recipe->RecipeIngredientOptions.Add(MakeGroup({ MakeIngredient(ItemA, 1) }));
	TestTrue(TEXT("Any satisfied option is enough"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(NoInventory, Recipe, Cache));

	TestFalse(TEXT("Null recipe is not satisfied"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(NoInventory, nullptr, Cache));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
