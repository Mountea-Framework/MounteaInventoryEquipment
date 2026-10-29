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
#include "Definitions/MounteaRecipeTemplate.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Statics/MounteaCraftingStatics.h"
#include "UObject/UnrealType.h"

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

		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		recipe->ResultItem = resultItem;
		recipe->RecipeIngredients.Add(DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe));
		IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe);

		const FMounteaCraftingResult result = IMounteaAdvancedCraftingParticipantInterface::Execute_StartCrafting(Participant, recipe);
		TestFalse(TEXT("Crafting rejected"), static_cast<bool>(result.bCraftingSuccess));

		const TArray<FMounteaInventoryItem> all = IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv);
		TestEqual(TEXT("Inventory unchanged: one stack"), all.Num(), 1);
		if (all.Num() == 1)
		{
			TestEqual(TEXT("Ingredient quantity unchanged"), all[0].GetQuantity(), 1);
		}
	});
}

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantFlatRequirementsTest, "FlatRequirementsAndRecipeOnlyCrafting")
bool FMounteaCraftingParticipantFlatRequirementsTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* ingredientItem = FInventoryWorldFixture::MakeTemplate(100);
		UMounteaInventoryItemTemplate* missingItem = FInventoryWorldFixture::MakeTemplate(100);
		UMounteaInventoryItemTemplate* resultItem = FInventoryWorldFixture::MakeTemplate(100);
		TestTrue(TEXT("Ingredient added"), IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(ingredientItem, 8)));
		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		recipe->ResultItem = resultItem;
		recipe->QuantityPerCreation = 2;
		UMounteaRecipeIngredient* ingredient = NewObject<UMounteaRecipeIngredient>(GetTransientPackage());
		ingredient->IngredientSource = ingredientItem;
		ingredient->RequiredQuantity = 2;
		recipe->RecipeIngredients.Add(DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe));
		ingredient->RequiredQuantity = 3;
		recipe->RecipeIngredients.Add(DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe));
		ingredient->IngredientSource = missingItem;
		ingredient->RequiredQuantity = 1;
		recipe->RecipeIngredients.Add(DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe));
		IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe);

		const TMap<UMounteaInventoryItemTemplate*, FIntPoint> quantities = UMounteaCraftingStatics::GetRecipeIngredientQuantities(Participant, recipe);
		TestEqual(TEXT("Every requirement is displayed"), quantities.Num(), 2);
		if (const FIntPoint* owned = quantities.Find(ingredientItem))
			TestEqual(TEXT("Display combines duplicate requirements"), *owned, FIntPoint(8, 5));
		else
			AddError(TEXT("Owned ingredient missing from display"));
		if (const FIntPoint* missing = quantities.Find(missingItem))
			TestEqual(TEXT("Missing ingredient remains visible with zero owned"), *missing, FIntPoint(0, 1));
		else
			AddError(TEXT("Missing ingredient omitted from display"));
		TestEqual(TEXT("Missing ingredient prevents crafting"), UMounteaCraftingStatics::GetMaximumCraftableUnits(Participant, recipe), 0);
		TestFalse(TEXT("Missing ingredient prevents availability"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsCraftingPossible(Participant, recipe));

		recipe->RecipeIngredients.Pop();
		TestEqual(TEXT("Output quantity scales complete creations"), UMounteaCraftingStatics::GetMaximumCraftableUnits(Participant, recipe), 2);
		TestTrue(TEXT("Known affordable recipe is available"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsCraftingPossible(Participant, recipe));
		const FMounteaCraftingResult result = UMounteaCraftingStatics::StartCrafting(Participant, recipe);
		TestTrue(TEXT("Recipe-only call crafts"), static_cast<bool>(result.bCraftingSuccess));
		const auto after = UMounteaCraftingStatics::GetRecipeIngredientQuantities(Participant, recipe);
		if (const FIntPoint* remaining = after.Find(ingredientItem))
			TestEqual(TEXT("Combined requirement consumed once"), remaining->X, 3);
		const TArray<FMounteaInventoryItem> outputs = IMounteaAdvancedInventoryInterface::Execute_FindItems(Inv, FInventoryItemSearchParams(resultItem));
		int32 outputQuantity = 0;
		for (const FMounteaInventoryItem& output : outputs)
			outputQuantity += output.Quantity;
		TestEqual(TEXT("Configured output quantity created"), outputQuantity, 2);
		TestEqual(TEXT("Remaining ingredients cannot afford another creation"), UMounteaCraftingStatics::GetMaximumCraftableUnits(Participant, recipe), 0);
		TestFalse(TEXT("Duplicate requirements reject before consuming any partial amount"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		const auto afterRejectedCraft = UMounteaCraftingStatics::GetRecipeIngredientQuantities(Participant, recipe);
		if (const FIntPoint* remaining = afterRejectedCraft.Find(ingredientItem))
			TestEqual(TEXT("Rejected duplicate craft preserves all remaining ingredients"), remaining->X, 3);
	});
}

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantRejectedRecipeTest, "RejectUnknownEmptyAndInvalidRecipes")
bool FMounteaCraftingParticipantRejectedRecipeTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* ingredientItem = FInventoryWorldFixture::MakeTemplate(100);
		UMounteaInventoryItemTemplate* resultItem = FInventoryWorldFixture::MakeTemplate(100);
		IMounteaAdvancedInventoryInterface::Execute_AddItem(Inv, FInventoryWorldFixture::MakeItem(ingredientItem, 3));
		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		recipe->ResultItem = resultItem;
		UMounteaRecipeIngredient* ingredient = NewObject<UMounteaRecipeIngredient>(GetTransientPackage());
		ingredient->IngredientSource = ingredientItem;
		recipe->RecipeIngredients.Add(DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe));
		TestFalse(TEXT("Unknown recipe cannot craft"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe);
		recipe->RecipeIngredients[0]->RequiredQuantity = 0;
		TestFalse(TEXT("Zero requirement cannot craft"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		recipe->RecipeIngredients[0]->RequiredQuantity = 1;
		recipe->QuantityPerCreation = 0;
		TestFalse(TEXT("Zero output quantity cannot craft"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		recipe->QuantityPerCreation = 1;
		recipe->RecipeIngredients.Add(nullptr);
		TestFalse(TEXT("Null ingredient cannot craft"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		TestFalse(TEXT("Null ingredient is unavailable"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsCraftingPossible(Participant, recipe));
		TestEqual(TEXT("Null ingredient maximum is zero"), UMounteaCraftingStatics::GetMaximumCraftableUnits(Participant, recipe), 0);
		recipe->RecipeIngredients.Reset();
		TestFalse(TEXT("Empty recipe cannot craft"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		TestFalse(TEXT("Empty recipe is unavailable"), IMounteaAdvancedCraftingParticipantInterface::Execute_IsCraftingPossible(Participant, recipe));
		TestEqual(TEXT("Empty recipe maximum is zero"), UMounteaCraftingStatics::GetMaximumCraftableUnits(Participant, recipe), 0);
		const TArray<FMounteaInventoryItem> all = IMounteaAdvancedInventoryInterface::Execute_GetAllItems(Inv);
		TestEqual(TEXT("Rejected crafts preserve the inventory"), all.Num(), 1);
		if (all.Num() == 1)
			TestEqual(TEXT("Rejected crafts consume nothing"), all[0].Quantity, 3);
	});
}

MOUNTEA_CRAFTING_PARTICIPANT_TEST(FMounteaCraftingParticipantMultipleStacksTest, "ConsumeRequirementAcrossStacks")
bool FMounteaCraftingParticipantMultipleStacksTest::RunTest(const FString& Parameters)
{
	return RunWithParticipant(*this, [this](UMounteaCraftingParticipantComponent* Participant, UMounteaInventoryComponent* Inv)
	{
		UMounteaInventoryItemTemplate* ingredientItem = FInventoryWorldFixture::MakeTemplate(100);
		UMounteaInventoryItemTemplate* resultItem = FInventoryWorldFixture::MakeTemplate(100);
		// Seed the serialized array to represent an inventory restored with separate matching stacks.
		FStructProperty* itemsProperty = FindFProperty<FStructProperty>(Inv->GetClass(), TEXT("InventoryItems"));
		if (!TestNotNull(TEXT("Serialized inventory array is available"), itemsProperty))
			return;
		FInventoryItemArray* items = itemsProperty->ContainerPtrToValuePtr<FInventoryItemArray>(Inv);
		for (const int32 quantity : { 2, 3 })
		{
			FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(ingredientItem, quantity);
			item.SetOwningInventory(Inv);
			items->Items.Add(item);
		}
		items->MarkArrayDirty();

		UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
		recipe->ResultItem = resultItem;
		UMounteaRecipeIngredient* ingredient = NewObject<UMounteaRecipeIngredient>(GetTransientPackage());
		ingredient->IngredientSource = ingredientItem;
		ingredient->RequiredQuantity = 4;
		recipe->RecipeIngredients.Add(DuplicateObject<UMounteaRecipeIngredient>(ingredient, recipe));
		IMounteaAdvancedCraftingParticipantInterface::Execute_LearnRecipe(Participant, recipe);
		TestEqual(TEXT("All stacks count toward availability"), UMounteaCraftingStatics::GetMaximumCraftableUnits(Participant, recipe), 1);
		TestTrue(TEXT("Requirement spanning stacks crafts"), static_cast<bool>(UMounteaCraftingStatics::StartCrafting(Participant, recipe).bCraftingSuccess));
		const auto remainingQuantities = UMounteaCraftingStatics::GetRecipeIngredientQuantities(Participant, recipe);
		if (const FIntPoint* remaining = remainingQuantities.Find(ingredientItem))
			TestEqual(TEXT("Exactly the full requirement is consumed across stacks"), remaining->X, 1);
		else
			AddError(TEXT("Remaining ingredient is missing"));
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
