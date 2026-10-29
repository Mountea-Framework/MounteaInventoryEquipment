// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
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
#include "Definitions/MounteaRecipeTemplate.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeIngredientDuplicationTest,
	"Mountea.InventorySystem.RecipeIngredients.DeepDuplication",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeIngredientDuplicationTest::RunTest(const FString& Parameters)
{
	UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
	UMounteaRecipeIngredient* ingredient = NewObject<UMounteaRecipeIngredient>(recipe);
	ingredient->IngredientSource = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
	ingredient->RequiredQuantity = 7;
	recipe->RecipeIngredients.Add(ingredient);

	UMounteaRecipeTemplate* duplicate = DuplicateObject<UMounteaRecipeTemplate>(recipe, GetTransientPackage());
	if (!TestNotNull(TEXT("Recipe duplicated"), duplicate)
		|| !TestEqual(TEXT("One flat ingredient row duplicated"), duplicate->RecipeIngredients.Num(), 1)
		|| !TestNotNull(TEXT("Ingredient duplicated"), duplicate->RecipeIngredients[0].Get()))
		return false;

	UMounteaRecipeIngredient* duplicatedIngredient = duplicate->RecipeIngredients[0];
	TestTrue(TEXT("Ingredient is a separate instance"), duplicatedIngredient != ingredient);
	TestEqual(TEXT("Ingredient belongs to duplicated recipe"), duplicatedIngredient->GetOuter(), static_cast<UObject*>(duplicate));
	TestEqual(TEXT("Ingredient class is retained"), duplicatedIngredient->GetClass(), ingredient->GetClass());
	TestTrue(TEXT("Ingredient source is retained"), duplicatedIngredient->IngredientSource == ingredient->IngredientSource);
	TestEqual(TEXT("Quantity is retained"), duplicatedIngredient->RequiredQuantity, 7);
	TestTrue(TEXT("Duplicate gets its own recipe identity"), duplicate->RecipeGuid != recipe->RecipeGuid);
	duplicatedIngredient->RequiredQuantity = 3;
	TestEqual(TEXT("Editing duplicate leaves source quantity intact"), ingredient->RequiredQuantity, 7);
	return true;
}

#if WITH_EDITOR

namespace
{
	void NotifyIngredientArrayChanged(UMounteaRecipeTemplate* Recipe, EPropertyChangeType::Type ChangeType, int32 Index = INDEX_NONE)
	{
		FProperty* property = FindFProperty<FArrayProperty>(UMounteaRecipeTemplate::StaticClass(), GET_MEMBER_NAME_CHECKED(UMounteaRecipeTemplate, RecipeIngredients));
		FEditPropertyChain chain;
		chain.AddHead(property);
		chain.SetActivePropertyNode(property);
		chain.SetActiveMemberPropertyNode(property);
		FPropertyChangedEvent changedEvent(property, ChangeType);
		TArray<TMap<FString, int32>> indices;
		if (Index != INDEX_NONE)
		{
			indices.Add({{ GET_MEMBER_NAME_STRING_CHECKED(UMounteaRecipeTemplate, RecipeIngredients), Index }});
			changedEvent.SetArrayIndexPerObject(indices);
			changedEvent.ObjectIteratorIndex = 0;
		}
		FPropertyChangedChainEvent chainEvent(chain, changedEvent);
		Recipe->PostEditChangeChainProperty(chainEvent);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeIngredientDefaultsTest,
	"Mountea.InventorySystem.RecipeIngredients.DefaultClassAndAddedRows",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeIngredientDefaultsTest::RunTest(const FString& Parameters)
{
	UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
	TestEqual(TEXT("Native ingredient class is the default"), recipe->DefaultIngredientClass.Get(), UMounteaRecipeIngredient::StaticClass());
	TestTrue(TEXT("Ingredients allow inline instances"), UMounteaRecipeIngredient::StaticClass()->HasAllClassFlags(CLASS_EditInlineNew | CLASS_DefaultToInstanced));
	FArrayProperty* arrayProperty = FindFProperty<FArrayProperty>(recipe->GetClass(), GET_MEMBER_NAME_CHECKED(UMounteaRecipeTemplate, RecipeIngredients));
	if (!TestNotNull(TEXT("Flat ingredient array is reflected"), arrayProperty))
		return false;
	TestTrue(TEXT("Array owns instanced references"), arrayProperty->HasAnyPropertyFlags(CPF_ContainsInstancedReference));

	recipe->RecipeIngredients.Add(nullptr);
	NotifyIngredientArrayChanged(recipe, EPropertyChangeType::ArrayAdd, 0);
	UMounteaRecipeIngredient* first = recipe->RecipeIngredients[0];
	if (!TestNotNull(TEXT("Adding a null row creates an ingredient"), first))
		return false;
	TestEqual(TEXT("Added ingredient uses native class"), first->GetClass(), UMounteaRecipeIngredient::StaticClass());
	TestEqual(TEXT("Added ingredient belongs to recipe"), first->GetOuter(), static_cast<UObject*>(recipe));
	TestEqual(TEXT("Added quantity defaults to one"), first->RequiredQuantity, 1);
	TestTrue(TEXT("Added ingredient participates in undo"), first->HasAnyFlags(RF_Transactional));

	first->RequiredQuantity = 5;
	NotifyIngredientArrayChanged(recipe, EPropertyChangeType::ArrayAdd, 0);
	TestTrue(TEXT("Existing ingredient instance is preserved"), recipe->RecipeIngredients[0] == first);
	TestEqual(TEXT("Existing values are preserved"), first->RequiredQuantity, 5);

	recipe->DefaultIngredientClass = nullptr;
	recipe->RecipeIngredients.Add(nullptr);
	NotifyIngredientArrayChanged(recipe, EPropertyChangeType::ArrayAdd);
	if (!TestNotNull(TEXT("Missing index finds newly added null row"), recipe->RecipeIngredients[1].Get()))
		return false;
	TestEqual(TEXT("Unset default falls back to native ingredient"), recipe->RecipeIngredients[1]->GetClass(), UMounteaRecipeIngredient::StaticClass());

	recipe->RecipeIngredients.Add(nullptr);
	NotifyIngredientArrayChanged(recipe, EPropertyChangeType::ValueSet, 2);
	TestNull(TEXT("Explicitly cleared rows stay empty until an add event"), recipe->RecipeIngredients[2].Get());
	NotifyIngredientArrayChanged(recipe, EPropertyChangeType::Duplicate, 2);
	TestNotNull(TEXT("Duplicated null row receives an instance"), recipe->RecipeIngredients[2].Get());
	return true;
}

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
