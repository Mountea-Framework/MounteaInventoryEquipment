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
#include "Components/MounteaInventoryComponent.h"
#include "Definitions/MounteaCraftingBaseDataTypes.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaRecipeIngredientsList.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"

namespace MounteaInventoryTest
{
	// Spawns a world + actor + inventory + participant (inventory first: participant BeginPlay looks it up), runs Body, tears down.
	template<typename TBody>
	bool RunWithParticipant(FAutomationTestBase& Test, TBody Body)
	{
		FInventoryWorldFixture fixture;
		if (!fixture.Setup(Test))
		{
			return false;
		}

		AActor* actor = fixture.SpawnActor();
		UMounteaInventoryComponent* inventory = fixture.AddComponent<UMounteaInventoryComponent>(actor);
		UMounteaCraftingParticipantComponent* participant = fixture.AddComponent<UMounteaCraftingParticipantComponent>(actor);
		fixture.Tick(1);
		if (Test.TestNotNull(TEXT("Inventory component created"), inventory) && Test.TestNotNull(TEXT("Participant component created"), participant))
		{
			Body(participant, inventory);
		}

		fixture.Tick(1);
		fixture.Teardown(Test);
		return true;
	}
}

using MounteaInventoryTest::FInventoryWorldFixture;
using MounteaInventoryTest::RunWithParticipant;

#define MOUNTEA_CRAFTING_PARTICIPANT_TEST(ClassName, Name) \
	IMPLEMENT_SIMPLE_AUTOMATION_TEST(ClassName, "Mountea.InventorySystem.CraftingParticipant." Name, \
		EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantLearnTest, "LearnRecipe")
bool FMounteaCraftingParticipantLearnTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent*)
	{
		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		TestFalse(TEXT("Unknown before learning"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsRecipeKnown(Participant, recipe));
		TestTrue(TEXT("LearnRecipe succeeds"), IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe));
		TestTrue(TEXT("Known after learning"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsRecipeKnown(Participant, recipe));
		TestTrue(TEXT("Guid stored"), Participant->KnownRecipes.Contains(recipe->RecipeGuid));
	});
}

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantForgetTest, "ForgetRecipe")
bool FMounteaCraftingParticipantForgetTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent*)
	{
		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe);
		TestTrue(TEXT("ForgetRecipe succeeds"), IMounteaAdvancedCraftingParticipantInterface::Execute_ForgetRecipe(Participant, recipe));
		TestFalse(TEXT("Unknown after forgetting"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsRecipeKnown(Participant, recipe));
		TestEqual(TEXT("No recipes stored"), Participant->KnownRecipes.Num(), 0);
	});
}

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantLearnTwiceTest, "LearnTwice")
bool FMounteaCraftingParticipantLearnTwiceTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent*)
	{
		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe);
		TestFalse(TEXT("Second learn rejected"), IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe));
		TestEqual(TEXT("Recipe stored once"), Participant->KnownRecipes.Num(), 1);
	});
}

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantMissingIngredientTest, "StartCraftingMissingIngredient")
bool FMounteaCraftingParticipantMissingIngredientTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* ingredientItem = FInventoryWorldFixture::MakeTemplate(10);
		UMounteaInventoryItemTemplate* resultItem = FInventoryWorldFixture::MakeTemplate(10);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(ingredientItem, 1));

		UMounteaRecipeIngredient* ingredient = NewObject<UMounteaRecipeIngredient>(GetTransientPackage());
		ingredient->IngredientSource = ingredientItem;
		ingredient->RequiredQuantity = 2; // inventory holds only 1

		UMounteaRecipeIngredientsList* ingredients = NewObject<UMounteaRecipeIngredientsList>(GetTransientPackage());
		ingredients->RecipeIngredients.Add(ingredient);

		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		recipe->ResultItem = resultItem;

		const FMounteaCraftingResult result = IMounteaAdvancedCraftingParticipantInterface::Execute_StartCrafting(Participant, recipe, ingredients);
		TestFalse(TEXT("Crafting rejected"), static_cast<bool>(result.bCraftingSuccess));

		const TArray<FMounteaInventoryItem> all = IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv);
		TestEqual(TEXT("Inventory unchanged: one stack"), all.Num(), 1);
		if (all.Num() == 1)
		{
			TestEqual(TEXT("Ingredient quantity unchanged"), all[0].GetQuantity(), 1);
		}
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
