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
#include "Definitions/MounteaRecipeTemplate.h"
#include "Statics/MounteaCraftingStatics.h"
#include "UObject/Package.h"

namespace
{
	// Pre-seeded counts isolate requirement arithmetic from inventory behavior.
	using FQuantityCache = TMap<UMounteaInventoryItemTemplate*, int32>;

	UMounteaRecipeIngredient* MakeIngredient(UMounteaInventoryItemTemplate* Source, const int32 Required)
	{
		UMounteaRecipeIngredient* ingredient = NewObject<UMounteaRecipeIngredient>(GetTransientPackage());
		ingredient->IngredientSource = Source;
		ingredient->RequiredQuantity = Required;
		return ingredient;
	}

	UMounteaRecipeTemplate* MakeRecipe(const TArray<UMounteaRecipeIngredient*>& Ingredients)
	{
		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		for (UMounteaRecipeIngredient* ingredient : Ingredients)
			recipe->RecipeIngredients.Add(ingredient ? DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe) : nullptr);
		return recipe;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStaticsRecipeSatisfiedTest, "Mountea.InventorySystem.CraftingStatics.IsRecipeSatisfiedByIngredients",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStaticsRecipeSatisfiedTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* itemA = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
	UMounteaInventoryItemTemplate* itemB = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
	UMounteaRecipeTemplate* recipe = MakeRecipe({ MakeIngredient(itemA, 2), MakeIngredient(itemB, 1) });
	const TScriptInterface<IMounteaAdvancedInventoryInterface> noInventory;
	FQuantityCache cache;
	cache.Add(itemA, 2);
	cache.Add(itemB, 1);
	TestTrue(TEXT("Exact quantities satisfy every ingredient"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, recipe, cache));
	cache.Add(itemA, 10);
	TestTrue(TEXT("Surplus satisfies"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, recipe, cache));
	cache.Add(itemB, 0);
	TestFalse(TEXT("One missing ingredient rejects the recipe"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, recipe, cache));
	TestFalse(TEXT("Null recipe rejects"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, nullptr, cache));
	TestFalse(TEXT("Empty recipe rejects"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, MakeRecipe({}), cache));
	TestEqual(TEXT("Empty recipe has no craftable creations"), UMounteaCraftingStatics::GetRecipeCraftableCreations(noInventory, MakeRecipe({}), cache), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStaticsDuplicateRequirementsTest, "Mountea.InventorySystem.CraftingStatics.DuplicateRequirements",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStaticsDuplicateRequirementsTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* item = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
	UMounteaRecipeTemplate* recipe = MakeRecipe({ MakeIngredient(item, 2), MakeIngredient(item, 3) });
	FQuantityCache requirements;
	TestTrue(TEXT("Duplicate requirements are valid"), UMounteaCraftingStatics::GetRecipeRequiredQuantities(recipe, requirements));
	TestEqual(TEXT("Duplicates produce one requirement"), requirements.Num(), 1);
	TestEqual(TEXT("Duplicate quantities sum"), requirements.FindRef(item), 5);

	const TScriptInterface<IMounteaAdvancedInventoryInterface> noInventory;
	FQuantityCache cache;
	cache.Add(item, 3);
	TestFalse(TEXT("Individually affordable rows still require their combined total"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, recipe, cache));
	cache.Add(item, 14);
	TestEqual(TEXT("Maximum uses the combined requirement"), UMounteaCraftingStatics::GetRecipeCraftableCreations(noInventory, recipe, cache), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingStaticsInvalidRequirementsTest, "Mountea.InventorySystem.CraftingStatics.InvalidRequirements",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaCraftingStaticsInvalidRequirementsTest::RunTest(const FString& Parameters)
{
	UMounteaInventoryItemTemplate* item = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
	FQuantityCache requirements;
	const TScriptInterface<IMounteaAdvancedInventoryInterface> noInventory;
	FQuantityCache cache;
	cache.Add(item, 10);
	for (const int32 quantity : { 0, -1 })
	{
		UMounteaRecipeTemplate* recipe = MakeRecipe({ MakeIngredient(item, 1), MakeIngredient(item, quantity) });
		TestFalse(TEXT("Nonpositive quantity invalidates the whole recipe"), UMounteaCraftingStatics::GetRecipeRequiredQuantities(recipe, requirements));
		TestTrue(TEXT("Invalid requirements never expose a partial map"), requirements.IsEmpty());
		TestFalse(TEXT("Invalid quantity cannot satisfy availability"), UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(noInventory, recipe, cache));
	}
	TestFalse(TEXT("Null ingredient rejects the whole recipe"), UMounteaCraftingStatics::GetRecipeRequiredQuantities(MakeRecipe({ MakeIngredient(item, 1), nullptr }), requirements));
	TestTrue(TEXT("Null ingredient does not expose partial requirements"), requirements.IsEmpty());
	TestEqual(TEXT("Null ingredient has no craftable creations"), UMounteaCraftingStatics::GetRecipeCraftableCreations(noInventory, MakeRecipe({ nullptr }), cache), 0);
	TestFalse(TEXT("Null source rejects"), UMounteaCraftingStatics::GetRecipeRequiredQuantities(MakeRecipe({ MakeIngredient(nullptr, 1) }), requirements));
	TestFalse(TEXT("Overflowing duplicate requirement rejects"), UMounteaCraftingStatics::GetRecipeRequiredQuantities(MakeRecipe({ MakeIngredient(item, MAX_int32), MakeIngredient(item, 1) }), requirements));
	TestTrue(TEXT("Overflow does not expose partial requirements"), requirements.IsEmpty());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
