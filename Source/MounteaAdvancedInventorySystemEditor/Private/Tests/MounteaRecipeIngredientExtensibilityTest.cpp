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

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Dom/JsonObject.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeIngredientExtensibilityTest,
    "Mountea.InventorySystem.RecipeIngredients.BlueprintExtensibility",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeIngredientExtensibilityTest::RunTest(const FString& Parameters)
{
    if (!TestTrue(TEXT("Ingredient supports Blueprint subclasses"), FKismetEditorUtilities::CanCreateBlueprintOfClass(UMounteaRecipeIngredient::StaticClass())))
        return false;
    const FName blueprintName(*(TEXT("BP_RecipeIngredientTest_") + FGuid::NewGuid().ToString(EGuidFormats::Digits)));
    UBlueprint* blueprint = FKismetEditorUtilities::CreateBlueprint(UMounteaRecipeIngredient::StaticClass(), GetTransientPackage(), blueprintName,
        BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
    if (!TestNotNull(TEXT("Transient ingredient Blueprint created"), blueprint))
        return false;

    const FName extraPropertyName(TEXT("CustomIngredientPotency"));
    FEdGraphPinType pinType;
    pinType.PinCategory = UEdGraphSchema_K2::PC_Int;
    if (!TestTrue(TEXT("Ingredient Blueprint accepts a custom field"), FBlueprintEditorUtils::AddMemberVariable(blueprint, extraPropertyName, pinType, TEXT("17"))))
        return false;
    for (FBPVariableDescription& variable : blueprint->NewVariables)
        if (variable.VarName == extraPropertyName)
            variable.PropertyFlags |= CPF_Edit | CPF_BlueprintVisible;
    FKismetEditorUtilities::CompileBlueprint(blueprint);
    if (!TestNotNull(TEXT("Ingredient Blueprint generated class exists"), blueprint->GeneratedClass.Get())
        || !TestTrue(TEXT("Ingredient Blueprint compiles"), blueprint->Status != BS_Error))
        return false;
    FIntProperty* extraProperty = FindFProperty<FIntProperty>(blueprint->GeneratedClass.Get(), extraPropertyName);
    if (!TestNotNull(TEXT("Custom property is reflected"), extraProperty))
        return false;
    TestTrue(TEXT("Custom property remains editable"), extraProperty->HasAnyPropertyFlags(CPF_Edit));

    UMounteaRecipeTemplate* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
    recipe->DefaultIngredientClass = blueprint->GeneratedClass.Get();
    recipe->RecipeIngredients.Add(nullptr);
    FProperty* ingredientArray = FindFProperty<FArrayProperty>(recipe->GetClass(), GET_MEMBER_NAME_CHECKED(UMounteaRecipeTemplate, RecipeIngredients));
    FEditPropertyChain chain;
    chain.AddHead(ingredientArray);
    chain.SetActivePropertyNode(ingredientArray);
    chain.SetActiveMemberPropertyNode(ingredientArray);
    FPropertyChangedEvent changeEvent(ingredientArray, EPropertyChangeType::ArrayAdd);
    FPropertyChangedChainEvent chainEvent(chain, changeEvent);
    recipe->PostEditChangeChainProperty(chainEvent);
    UMounteaRecipeIngredient* ingredient = recipe->RecipeIngredients[0];
    if (!TestNotNull(TEXT("Added row instantiates the selected subclass"), ingredient)
        || !TestEqual(TEXT("Added row uses Blueprint class"), ingredient->GetClass(), blueprint->GeneratedClass.Get()))
        return false;
    TestEqual(TEXT("Blueprint custom default is retained"), *extraProperty->ContainerPtrToValuePtr<int32>(ingredient), 17);
    *extraProperty->ContainerPtrToValuePtr<int32>(ingredient) = 42;
    ingredient->RequiredQuantity = 2;
    recipe->DefaultIngredientClass = UMounteaRecipeIngredient::StaticClass();
    recipe->PostEditChangeChainProperty(chainEvent);
    TestTrue(TEXT("Changing default does not replace an existing subclass"), recipe->RecipeIngredients[0] == ingredient);

    UMounteaRecipeTemplate* duplicate = DuplicateObject<UMounteaRecipeTemplate>(recipe, GetTransientPackage());
    if (!TestNotNull(TEXT("Recipe duplicates"), duplicate)
        || !TestEqual(TEXT("Duplication preserves one flat row"), duplicate->RecipeIngredients.Num(), 1)
        || !TestNotNull(TEXT("Subclass ingredient duplicates"), duplicate->RecipeIngredients[0].Get()))
        return false;
    UMounteaRecipeIngredient* duplicatedIngredient = duplicate->RecipeIngredients[0];
    if (!TestEqual(TEXT("Duplication retains Blueprint subclass"), duplicatedIngredient->GetClass(), blueprint->GeneratedClass.Get()))
        return false;
    TestTrue(TEXT("Duplicated subclass is a separate object"), duplicatedIngredient != ingredient);
    TestEqual(TEXT("Duplicated subclass belongs to new recipe"), duplicatedIngredient->GetOuter(), static_cast<UObject*>(duplicate));
    TestEqual(TEXT("Duplication preserves custom value"), *extraProperty->ContainerPtrToValuePtr<int32>(duplicatedIngredient), 42);
    *extraProperty->ContainerPtrToValuePtr<int32>(duplicatedIngredient) = 99;
    TestEqual(TEXT("Duplicate custom data is independent"), *extraProperty->ContainerPtrToValuePtr<int32>(ingredient), 42);

    UMounteaInventoryItemTemplate* source = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
    UMounteaInventoryItemTemplate* result = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
    source->Guid = FGuid::NewGuid();
    result->Guid = FGuid::NewGuid();
    ingredient->IngredientSource = source;
    recipe->ResultItem = result;
    const auto json = MakeShared<FJsonObject>();
    json->SetStringField(TEXT("guid"), recipe->RecipeGuid.ToString(EGuidFormats::DigitsWithHyphens));
    json->SetStringField(TEXT("name"), TEXT("SubclassRecipe"));
    json->SetStringField(TEXT("ingredientModel"), TEXT("flat-v1"));
    json->SetNumberField(TEXT("qtyMax"), 1);
    const auto resultJson = MakeShared<FJsonObject>();
    resultJson->SetStringField(TEXT("itemRef"), result->Guid.ToString(EGuidFormats::DigitsWithHyphens));
    json->SetObjectField(TEXT("result"), resultJson);
    const auto requirements = MakeShared<FJsonObject>();
    requirements->SetNumberField(TEXT("duration"), 0);
    json->SetObjectField(TEXT("reqs"), requirements);
    const auto ingredientJson = MakeShared<FJsonObject>();
    const auto ingredientRef = MakeShared<FJsonObject>();
    ingredientRef->SetStringField(TEXT("guid"), source->Guid.ToString(EGuidFormats::DigitsWithHyphens));
    ingredientJson->SetObjectField(TEXT("ref"), ingredientRef);
    ingredientJson->SetNumberField(TEXT("qty"), 4);
    json->SetArrayField(TEXT("ingredients"), { MakeShared<FJsonValueObject>(ingredientJson) });
    const TMap<FGuid, UMounteaInventoryItemTemplate*> items = { { source->Guid, source }, { result->Guid, result } };
    FString error;
    if (!TestTrue(TEXT("Reimport succeeds"), UMounteaAdvancedInventoryRecipeEditorStatics::DeserializeRecipeFromJson(json, recipe, items, {}, error)))
    {
        AddError(error);
        return false;
    }
    if (!TestEqual(TEXT("Reimport retains one row"), recipe->RecipeIngredients.Num(), 1)
        || !TestNotNull(TEXT("Reimport retains an instance"), recipe->RecipeIngredients[0].Get())
        || !TestEqual(TEXT("Reimport retains the existing subclass"), recipe->RecipeIngredients[0]->GetClass(), blueprint->GeneratedClass.Get()))
        return false;
    TestEqual(TEXT("Reimport updates required quantity"), recipe->RecipeIngredients[0]->RequiredQuantity, 4);
    TestEqual(TEXT("Reimport preserves custom field value"), *extraProperty->ContainerPtrToValuePtr<int32>(recipe->RecipeIngredients[0].Get()), 42);
    TestEqual(TEXT("Reimport owns its subclass instance"), recipe->RecipeIngredients[0]->GetOuter(), static_cast<UObject*>(recipe));

    UMounteaRecipeTemplate* freshRecipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
    freshRecipe->DefaultIngredientClass = blueprint->GeneratedClass.Get();
    if (!TestTrue(TEXT("Fresh import uses chosen default class"), UMounteaAdvancedInventoryRecipeEditorStatics::DeserializeRecipeFromJson(json, freshRecipe, items, {}, error)))
    {
        AddError(error);
        return false;
    }
    if (!TestEqual(TEXT("Fresh import creates one row"), freshRecipe->RecipeIngredients.Num(), 1)
        || !TestNotNull(TEXT("Fresh import creates ingredient"), freshRecipe->RecipeIngredients[0].Get())
        || !TestEqual(TEXT("Fresh import uses Blueprint subclass"), freshRecipe->RecipeIngredients[0]->GetClass(), blueprint->GeneratedClass.Get()))
        return false;
    TestEqual(TEXT("Fresh import keeps subclass defaults"), *extraProperty->ContainerPtrToValuePtr<int32>(freshRecipe->RecipeIngredients[0].Get()), 17);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
