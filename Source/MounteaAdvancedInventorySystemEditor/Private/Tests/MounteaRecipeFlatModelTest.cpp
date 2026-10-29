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

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "ObjectTools.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

namespace MounteaRecipeFlatModelTest
{
    using FRecipeStatics = UMounteaAdvancedInventoryRecipeEditorStatics;

    struct FFixture
    {
        UMounteaInventoryItemTemplate* Ingredient = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
        UMounteaInventoryItemTemplate* Result = NewObject<UMounteaInventoryItemTemplate>(GetTransientPackage());
        FGuid RecipeGuid = FGuid::NewGuid();

        FFixture()
        {
            Ingredient->Guid = FGuid::NewGuid();
            Ingredient->DisplayName = FText::FromString(TEXT("Flat model test ingredient"));
            Result->Guid = FGuid::NewGuid();
            Result->DisplayName = FText::FromString(TEXT("Flat model test result"));
        }

        TSet<FGuid> KnownGuids() const { return { Ingredient->Guid, Result->Guid }; }
        TMap<FGuid, UMounteaInventoryItemTemplate*> Items() const
        {
            return { { Ingredient->Guid, Ingredient }, { Result->Guid, Result } };
        }

        TSharedRef<FJsonObject> IngredientJson(const double Quantity) const
        {
            const auto ref = MakeShared<FJsonObject>();
            ref->SetStringField(TEXT("guid"), Ingredient->Guid.ToString(EGuidFormats::DigitsWithHyphens));
            const auto row = MakeShared<FJsonObject>();
            row->SetObjectField(TEXT("ref"), ref);
            row->SetNumberField(TEXT("qty"), Quantity);
            return row;
        }

        TSharedRef<FJsonObject> Json() const
        {
            const auto json = MakeShared<FJsonObject>();
            json->SetStringField(TEXT("guid"), RecipeGuid.ToString(EGuidFormats::DigitsWithHyphens));
            json->SetStringField(TEXT("name"), TEXT("FlatModelTest"));
            json->SetNumberField(TEXT("qtyMax"), 3);
            const auto result = MakeShared<FJsonObject>();
            result->SetStringField(TEXT("itemRef"), Result->Guid.ToString(EGuidFormats::DigitsWithHyphens));
            json->SetObjectField(TEXT("result"), result);
            const auto requirements = MakeShared<FJsonObject>();
            requirements->SetNumberField(TEXT("duration"), 1.75);
            requirements->SetStringField(TEXT("station"), TEXT("Test Forge"));
            json->SetObjectField(TEXT("reqs"), requirements);
            json->SetArrayField(TEXT("ingredients"), {
                MakeShared<FJsonValueObject>(IngredientJson(2)), MakeShared<FJsonValueObject>(IngredientJson(3)) });
            return json;
        }

        UMounteaRecipeTemplate* Recipe() const
        {
            auto* recipe = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
            recipe->RecipeGuid = RecipeGuid;
            recipe->ResultItem = Result;
            recipe->QuantityPerCreation = 3;
            recipe->CraftingTime = 1.75f;
            recipe->RequiredCraftingPlace = FGameplayTag::RequestGameplayTag(TEXT("Mountea_Inventory.Crafting.Blacksmith"), false);
            for (const int32 quantity : { 2, 3 })
            {
                auto* ingredient = NewObject<UMounteaRecipeIngredient>(recipe);
                ingredient->IngredientSource = Ingredient;
                ingredient->RequiredQuantity = quantity;
                recipe->RecipeIngredients.Add(ingredient);
            }
            return recipe;
        }
    };

    TSharedRef<FJsonObject> LegacyGroup(const TArray<TSharedPtr<FJsonValue>>& Ingredients, const bool Required = true, const bool OneOf = false)
    {
        const auto group = MakeShared<FJsonObject>();
        group->SetBoolField(TEXT("required"), Required);
        group->SetBoolField(TEXT("oneOf"), OneOf);
        group->SetArrayField(TEXT("ingredients"), Ingredients);
        return group;
    }

    TArray<uint8> JsonBytes(const TSharedRef<FJsonObject>& Json)
    {
        FString text;
        FJsonSerializer::Serialize(Json, TJsonWriterFactory<>::Create(&text));
        const FTCHARToUTF8 utf8(*text);
        TArray<uint8> bytes;
        bytes.Append(reinterpret_cast<const uint8*>(utf8.Get()), utf8.Length());
        return bytes;
    }

    bool CheckRestored(FAutomationTestBase& Test, const UMounteaRecipeTemplate* Restored,
        const UMounteaRecipeTemplate* Original, const UMounteaInventoryItemTemplate* Ingredient)
    {
        bool valid = Test.TestEqual(TEXT("Recipe GUID preserved"), Restored->RecipeGuid, Original->RecipeGuid);
        valid &= Test.TestEqual(TEXT("Result preserved"), Restored->ResultItem.Get(), Original->ResultItem.Get());
        valid &= Test.TestEqual(TEXT("Output quantity preserved"), Restored->QuantityPerCreation, Original->QuantityPerCreation);
        valid &= Test.TestEqual(TEXT("Crafting duration preserved"), Restored->CraftingTime, Original->CraftingTime);
        valid &= Test.TestEqual(TEXT("Station preserved"), Restored->RequiredCraftingPlace, Original->RequiredCraftingPlace);
        valid &= Test.TestEqual(TEXT("Duplicate rows normalized to one requirement"), Restored->RecipeIngredients.Num(), 1);
        if (Restored->RecipeIngredients.Num() == 1)
        {
            valid &= Test.TestTrue(TEXT("Ingredient reference preserved"), Restored->RecipeIngredients[0]->IngredientSource.Get() == Ingredient);
            valid &= Test.TestEqual(TEXT("Duplicate quantities combined"), Restored->RecipeIngredients[0]->RequiredQuantity, 5);
        }
        return valid;
    }

    bool CheckArchiveRoundTrip(FAutomationTestBase& Test, const TArray<uint8>& Bytes,
        const FFixture& Fixture, const UMounteaRecipeTemplate* Original)
    {
        FString recipeText, stationText, error;
        TArray<FString> bundledItems;
        if (!Test.TestTrue(TEXT("Parse exported recipe ZIP"), FRecipeStatics::ParseRecipeFromZip(Bytes, recipeText, bundledItems, stationText, error)))
        {
            Test.AddError(error);
            return false;
        }
        Test.TestEqual(TEXT("Result and ingredient items are bundled once"), bundledItems.Num(), 2);
        TSet<FGuid> exportedItemGuids;
        for (const FString& itemText : bundledItems)
        {
            TSharedPtr<FJsonObject> item;
            if (!Test.TestTrue(TEXT("Bundled item is JSON"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(itemText), item) && item.IsValid()))
                return false;
            FGuid guid;
            Test.TestTrue(TEXT("Bundled item GUID is valid"), FGuid::Parse(item->GetStringField(TEXT("guid")), guid));
            exportedItemGuids.Add(guid);
        }
        Test.TestTrue(TEXT("Ingredient item is included"), exportedItemGuids.Contains(Fixture.Ingredient->Guid));
        Test.TestTrue(TEXT("Result item is included"), exportedItemGuids.Contains(Fixture.Result->Guid));
        TSharedPtr<FJsonObject> json;
        if (!Test.TestTrue(TEXT("Recipe is JSON"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(recipeText), json) && json.IsValid()))
            return false;
        Test.TestFalse(TEXT("Export contains no legacy groups"), json->HasField(TEXT("groups")));
        Test.TestFalse(TEXT("Export needs no ingredient model discriminator"), json->HasField(TEXT("ingredientModel")));
        Test.TestEqual(TEXT("Export combines duplicate rows"), json->GetArrayField(TEXT("ingredients")).Num(), 1);
        if (!Test.TestTrue(TEXT("Exported JSON validates"), FRecipeStatics::ValidateRecipe(json, Fixture.KnownGuids(), error)))
        {
            Test.AddError(error);
            return false;
        }
        auto* restored = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
        if (!Test.TestTrue(TEXT("Reimport into transient recipe"), FRecipeStatics::DeserializeRecipeFromJson(json, restored, Fixture.Items(), {}, error)))
        {
            Test.AddError(error);
            return false;
        }
        return CheckRestored(Test, restored, Original, Fixture.Ingredient);
    }

    struct FExportFiles
    {
        FString Directory = FPaths::ProjectSavedDir() / TEXT("Automation/MounteaRecipeFlatModel") / FGuid::NewGuid().ToString(EGuidFormats::Digits);
        TArray<FString> Files;
        ~FExportFiles()
        {
            for (const FString& file : Files) IFileManager::Get().Delete(*file);
            IFileManager::Get().DeleteDirectory(*Directory);
        }
        FString File(const TCHAR* Name)
        {
            const FString file = Directory / Name;
            Files.Add(file);
            return file;
        }
    };

    struct FImportedAssets
    {
        FAutomationTestBase& Test;
        const FString Folder = TEXT("/Game/MounteaInventoryTests/Import/") + FGuid::NewGuid().ToString(EGuidFormats::Digits);

        TArray<FAssetData> Assets() const
        {
            auto& registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
            registry.ScanPathsSynchronous({ Folder }, true);
            TArray<FAssetData> assets;
            registry.GetAssetsByPath(FName(*Folder), assets, true);
            return assets;
        }

        ~FImportedAssets()
        {
            // The immutable folder is generated by this fixture; never delete the shared test root.
            const FString root = TEXT("/Game/MounteaInventoryTests/Import/");
            if (!Folder.StartsWith(root) || Folder.Len() != root.Len() + 32)
            { Test.AddError(TEXT("Refusing cleanup outside the isolated recipe test folder")); return; }
            const TArray<FAssetData> assets = Assets();
            for (const FAssetData& asset : assets)
                if (!asset.PackageName.ToString().StartsWith(Folder + TEXT("/")))
                { Test.AddError(TEXT("Refusing cleanup of an asset outside the recipe test folder")); return; }
            if (!assets.IsEmpty()) ObjectTools::DeleteAssets(assets, false);
            // Remove any package files before rescanning so deleted assets cannot be rediscovered.
            const FString directory = FPackageName::LongPackageNameToFilename(Folder);
            if (FPaths::DirectoryExists(directory) && !IFileManager::Get().DeleteDirectory(*directory, false, true))
            { Test.AddError(TEXT("Imported recipe test directory could not be removed")); return; }
            if (!Assets().IsEmpty())
                Test.AddError(TEXT("Imported recipe test assets could not be removed"));
        }
    };

}

using namespace MounteaRecipeFlatModelTest;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatDeserializeTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.CanonicalIngredientsOnly",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatDeserializeTest::RunTest(const FString& Parameters)
{
    FFixture fixture;
    FString error;
    const auto json = fixture.Json();
    auto* expected = fixture.Recipe();
    if (!TestTrue(TEXT("Registered test station exists"), expected->RequiredCraftingPlace.IsValid())) return false;
    const TMap<FString, FString> stations { { TEXT("Test Forge"), expected->RequiredCraftingPlace.ToString() } };
    TestTrue(TEXT("Canonical flat recipe validates"), FRecipeStatics::ValidateRecipe(json, fixture.KnownGuids(), error));
    auto* restored = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
    if (TestTrue(TEXT("Canonical flat recipe deserializes"), FRecipeStatics::DeserializeRecipeFromJson(json, restored, fixture.Items(), stations, error)))
        CheckRestored(*this, restored, expected, fixture.Ingredient);

    const auto unresolvedStation = fixture.Json();
    unresolvedStation->GetObjectField(TEXT("reqs"))->SetStringField(TEXT("station"), TEXT("Mountea_Unregistered_Exchange_Test_Station"));
    const FGuid previousGuid = restored->RecipeGuid;
    TestFalse(TEXT("Unresolved station rejects before changing recipe"), FRecipeStatics::DeserializeRecipeFromJson(unresolvedStation, restored, fixture.Items(), {}, error));
    TestEqual(TEXT("Unresolved station preserves recipe identity"), restored->RecipeGuid, previousGuid);

    const auto obsolete = fixture.Json();
    obsolete->RemoveField(TEXT("ingredients"));
    obsolete->SetArrayField(TEXT("groups"), {
        MakeShared<FJsonValueObject>(LegacyGroup({ MakeShared<FJsonValueObject>(fixture.IngredientJson(2)) })) });
    TestFalse(TEXT("Group-based recipes are rejected"), FRecipeStatics::ValidateRecipe(obsolete, fixture.KnownGuids(), error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatRejectTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.ValidationAndAtomicDeserialize",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatRejectTest::RunTest(const FString& Parameters)
{
    FFixture fixture;
    auto* unchanged = fixture.Recipe();
    // Distinct sentinel values detect partial mutation before a later validation failure.
    unchanged->RecipeGuid = FGuid::NewGuid();
    unchanged->ResultItem = fixture.Ingredient;
    unchanged->QuantityPerCreation = 11;
    unchanged->CraftingTime = 9.25f;
    unchanged->RecipeIngredients[0]->IngredientSource = fixture.Result;
    unchanged->RecipeIngredients[0]->RequiredQuantity = 7;
    unchanged->RecipeIngredients[1]->IngredientSource = fixture.Result;
    unchanged->RecipeIngredients[1]->RequiredQuantity = 8;
    const FGuid originalGuid = unchanged->RecipeGuid;
    const auto originalStation = unchanged->RequiredCraftingPlace;
    const auto reject = [this, &fixture, unchanged, originalGuid, originalStation](const TCHAR* Label, const TSharedPtr<FJsonObject>& Json)
    {
        FString error;
        TestFalse(Label, FRecipeStatics::ValidateRecipe(Json, fixture.KnownGuids(), error));
        TestFalse(TEXT("Rejected JSON has an explanation"), error.IsEmpty());
        TestFalse(TEXT("Invalid deserialize fails"), FRecipeStatics::DeserializeRecipeFromJson(Json, unchanged, fixture.Items(), {}, error));
        TestEqual(TEXT("Failure preserves GUID"), unchanged->RecipeGuid, originalGuid);
        TestTrue(TEXT("Failure preserves result"), unchanged->ResultItem.Get() == fixture.Ingredient);
        TestEqual(TEXT("Failure preserves yield"), unchanged->QuantityPerCreation, 11);
        TestEqual(TEXT("Failure preserves duration"), unchanged->CraftingTime, 9.25f);
        TestEqual(TEXT("Failure preserves station"), unchanged->RequiredCraftingPlace, originalStation);
        TestEqual(TEXT("Failure preserves original rows"), unchanged->RecipeIngredients.Num(), 2);
        if (unchanged->RecipeIngredients.Num() == 2)
        {
            TestEqual(TEXT("First original quantity preserved"), unchanged->RecipeIngredients[0]->RequiredQuantity, 7);
            TestEqual(TEXT("Second original quantity preserved"), unchanged->RecipeIngredients[1]->RequiredQuantity, 8);
            TestTrue(TEXT("First original reference preserved"), unchanged->RecipeIngredients[0]->IngredientSource.Get() == fixture.Result);
            TestTrue(TEXT("Second original reference preserved"), unchanged->RecipeIngredients[1]->IngredientSource.Get() == fixture.Result);
        }
    };

    reject(TEXT("Null JSON rejects"), nullptr);
    for (const double quantity : { 0., -1., 1.5, static_cast<double>(MAX_int32) + 1. })
    {
        const auto json = fixture.Json();
        json->SetArrayField(TEXT("ingredients"), { MakeShared<FJsonValueObject>(fixture.IngredientJson(quantity)) });
        reject(TEXT("Ingredient quantity must be a positive int32"), json);
    }
    {
        const auto json = fixture.Json();
        const auto row = fixture.IngredientJson(1);
        row->SetStringField(TEXT("qty"), TEXT("two"));
        json->SetArrayField(TEXT("ingredients"), { MakeShared<FJsonValueObject>(row) });
        reject(TEXT("String ingredient quantity rejects"), json);
    }
    for (const TCHAR* guid : { TEXT("invalid-guid"), TEXT("00000000-0000-0000-0000-000000000000") })
    {
        const auto json = fixture.Json();
        json->SetStringField(TEXT("guid"), guid);
        reject(TEXT("Malformed or zero recipe GUID rejects"), json);
    }
    for (const FString& guid : { FString(TEXT("bad-guid")), FGuid::NewGuid().ToString() })
    {
        const auto json = fixture.Json();
        json->GetObjectField(TEXT("result"))->SetStringField(TEXT("itemRef"), guid);
        reject(TEXT("Invalid or unresolved result rejects"), json);
    }
    {
        const auto json = fixture.Json();
        json->RemoveField(TEXT("result"));
        reject(TEXT("Missing result rejects"), json);
    }
    {
        const auto json = fixture.Json();
        json->SetArrayField(TEXT("ingredients"), {});
        reject(TEXT("Empty ingredients reject"), json);
    }
    {
        const auto json = fixture.Json();
        json->SetArrayField(TEXT("ingredients"), { MakeShared<FJsonValueString>(TEXT("malformed-row")) });
        reject(TEXT("Malformed ingredient row rejects"), json);
    }
    {
        const auto json = fixture.Json();
        const auto row = fixture.IngredientJson(1);
        row->GetObjectField(TEXT("ref"))->SetStringField(TEXT("guid"), FGuid::NewGuid().ToString());
        json->SetArrayField(TEXT("ingredients"), { MakeShared<FJsonValueObject>(row) });
        reject(TEXT("Unresolved ingredient GUID rejects"), json);
    }
    {
        const auto json = fixture.Json();
        json->SetArrayField(TEXT("ingredients"), {
            MakeShared<FJsonValueObject>(fixture.IngredientJson(MAX_int32)), MakeShared<FJsonValueObject>(fixture.IngredientJson(1)) });
        reject(TEXT("Combined ingredient quantity overflow rejects"), json);
    }
    for (const double quantity : { 0., 1.5 })
    {
        const auto json = fixture.Json();
        json->SetNumberField(TEXT("qtyMax"), quantity);
        reject(TEXT("Output quantity must be positive and integral"), json);
    }
    {
        const auto json = fixture.Json();
        json->SetArrayField(TEXT("groups"), {});
        reject(TEXT("Both ingredient model keys reject"), json);
    }
    for (const bool oneOf : { false, true })
    {
        const auto json = fixture.Json();
        json->RemoveField(TEXT("ingredients"));
        json->SetArrayField(TEXT("groups"), { MakeShared<FJsonValueObject>(LegacyGroup(
            { MakeShared<FJsonValueObject>(fixture.IngredientJson(1)) }, oneOf, oneOf)) });
        reject(oneOf ? TEXT("Legacy oneOf rejects") : TEXT("Optional legacy group rejects"), json);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatTemplateValidationTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.EditableTemplateValidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatTemplateValidationTest::RunTest(const FString& Parameters)
{
    FFixture fixture;
    FString error;
    auto* recipe = fixture.Recipe();
    TestTrue(TEXT("Valid duplicate requirements are editable"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->RecipeIngredients[0]->RequiredQuantity = MAX_int32;
    TestFalse(TEXT("Combined quantity overflow rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->RecipeIngredients[0]->RequiredQuantity = 0;
    TestFalse(TEXT("Zero quantity rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->RecipeIngredients[0]->RequiredQuantity = 2;
    recipe->RecipeIngredients[0]->IngredientSource = nullptr;
    TestFalse(TEXT("Missing ingredient asset rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->RecipeIngredients[0]->IngredientSource = fixture.Ingredient;
    recipe->QuantityPerCreation = 0;
    TestFalse(TEXT("Zero output quantity rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->QuantityPerCreation = 3;
    recipe->CraftingTime = -1;
    TestFalse(TEXT("Negative duration rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->CraftingTime = 1.75f;
    recipe->ResultItem = nullptr;
    TestFalse(TEXT("Missing result asset rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->ResultItem = fixture.Result;
    recipe->RecipeGuid = FGuid();
    TestFalse(TEXT("Zero recipe GUID rejects"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    recipe->RecipeGuid = fixture.RecipeGuid;
    recipe->RecipeIngredients.Reset();
    TestFalse(TEXT("Empty requirements reject"), FRecipeStatics::ValidateRecipeTemplate(recipe, error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatZipParserTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.InMemoryArchive",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatZipParserTest::RunTest(const FString& Parameters)
{
    FFixture fixture;
    auto* expected = fixture.Recipe();
    if (!TestTrue(TEXT("Registered test station exists"), expected->RequiredCraftingPlace.IsValid())) return false;
    const FString taxonomy = FString::Printf(TEXT("[{\"name\":\"Test Forge\",\"tag\":\"%s\"}]"), *expected->RequiredCraftingPlace.ToString());
    const FTCHARToUTF8 taxonomyUtf8(*taxonomy);
    TArray<uint8> taxonomyBytes;
    taxonomyBytes.Append(reinterpret_cast<const uint8*>(taxonomyUtf8.Get()), taxonomyUtf8.Length());
    TMap<FString, TArray<uint8>> entries;
    entries.Add(TEXT("recipe.json"), JsonBytes(fixture.Json()));
    entries.Add(TEXT("taxonomy/crafting-stations.json"), taxonomyBytes);
    const auto bundledItem = MakeShared<FJsonObject>();
    bundledItem->SetStringField(TEXT("guid"), fixture.Ingredient->Guid.ToString(EGuidFormats::DigitsWithHyphens));
    entries.Add(TEXT("items/") + fixture.Ingredient->Guid.ToString(EGuidFormats::DigitsWithHyphens) + TEXT(".json"), JsonBytes(bundledItem));
    TArray<uint8> archive;
    if (!TestTrue(TEXT("Build in-memory archive"), FMounteaInventoryZipHelper::BuildArchive(entries, archive))) return false;
    FString recipeText, stationText, error;
    TArray<FString> items;
    if (!TestTrue(TEXT("Parse in-memory recipe archive"), FRecipeStatics::ParseRecipeFromZip(archive, recipeText, items, stationText, error))) return false;
    TestEqual(TEXT("Taxonomy preserved"), stationText, taxonomy);
    TestEqual(TEXT("Bundled item extracted"), items.Num(), 1);
    TSharedPtr<FJsonObject> json;
    if (!TestTrue(TEXT("Recipe text parses"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(recipeText), json))) return false;
    auto* restored = NewObject<UMounteaRecipeTemplate>(GetTransientPackage());
    if (TestTrue(TEXT("Parsed recipe deserializes"), FRecipeStatics::DeserializeRecipeFromJson(json, restored, fixture.Items(), FRecipeStatics::ParseStationTagMap(stationText), error)))
        CheckRestored(*this, restored, expected, fixture.Ingredient);
    entries.Remove(TEXT("recipe.json"));
    archive.Reset();
    if (TestTrue(TEXT("Build archive missing primary JSON"), FMounteaInventoryZipHelper::BuildArchive(entries, archive)))
    {
        recipeText.Reset(); stationText.Reset(); items.Reset();
        TestFalse(TEXT("Missing recipe.json rejects"), FRecipeStatics::ParseRecipeFromZip(archive, recipeText, items, stationText, error));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatExportRoundTripTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.SingleAndBundleExportRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatExportRoundTripTest::RunTest(const FString& Parameters)
{
    // All recipes/items remain transient. Only export archives are written to this unique Saved directory.
    FFixture fixture;
    auto* first = fixture.Recipe();
    auto* second = fixture.Recipe();
    second->RecipeGuid = FGuid::NewGuid();
    second->QuantityPerCreation = 7;
    second->CraftingTime = 12.5f;
    second->RequiredCraftingPlace = FGameplayTag();
    if (!TestTrue(TEXT("Registered test station exists"), first->RequiredCraftingPlace.IsValid())) return false;
    FExportFiles files;
    if (!TestTrue(TEXT("Create isolated export directory"), IFileManager::Get().MakeDirectory(*files.Directory, true))) return false;
    FString error;
    const FString singlePath = files.File(TEXT("single.mntearecipe"));
    if (!TestTrue(TEXT("Export single recipe"), FRecipeStatics::ExportRecipesToFilePath({ first }, singlePath, error)))
    { AddError(error); return false; }
    TArray<uint8> single;
    if (TestTrue(TEXT("Read single recipe export"), FMounteaInventoryZipHelper::LoadFileToBytes(singlePath, single)))
        CheckArchiveRoundTrip(*this, single, fixture, first);
    const FString bundlePath = files.File(TEXT("bundle.mntearecipes"));
    if (!TestTrue(TEXT("Export recipe bundle"), FRecipeStatics::ExportRecipesToFilePath({ first, second }, bundlePath, error)))
    { AddError(error); return false; }
    TArray<uint8> bundle;
    if (!TestTrue(TEXT("Read bundle export"), FMounteaInventoryZipHelper::LoadFileToBytes(bundlePath, bundle))) return false;
    TArray<TArray<uint8>> inner;
    if (!TestTrue(TEXT("Expand exported recipe bundle"), FMounteaInventoryZipHelper::ExpandBundle(bundle, TEXT("mntearecipe"), inner))) return false;
    TestEqual(TEXT("Bundle contains both recipes"), inner.Num(), 2);
    TSet<FGuid> seenGuids;
    for (const TArray<uint8>& bytes : inner)
    {
        FString recipeText, stationText;
        TArray<FString> items;
        if (!TestTrue(TEXT("Read bundled recipe identity"), FRecipeStatics::ParseRecipeFromZip(bytes, recipeText, items, stationText, error))) continue;
        const FGuid guid = FRecipeStatics::ExtractGuidFromRecipeJson(recipeText);
        seenGuids.Add(guid);
        const UMounteaRecipeTemplate* expected = guid == first->RecipeGuid ? first : guid == second->RecipeGuid ? second : nullptr;
        if (TestNotNull(TEXT("Bundle recipe GUID matches selection"), expected))
            CheckArchiveRoundTrip(*this, bytes, fixture, expected);
    }
    TestTrue(TEXT("First recipe found in bundle"), seenGuids.Contains(first->RecipeGuid));
    TestTrue(TEXT("Second recipe found in bundle"), seenGuids.Contains(second->RecipeGuid));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatDuplicateIdentityTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.DuplicateIdentity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatDuplicateIdentityTest::RunTest(const FString& Parameters)
{
    FFixture fixture;
    auto* source = fixture.Recipe();
    auto* normal = DuplicateObject<UMounteaRecipeTemplate>(source, GetTransientPackage(), FName(*FGuid::NewGuid().ToString(EGuidFormats::Digits)));
    if (TestNotNull(TEXT("Normal duplication creates a recipe"), normal))
    {
        TestTrue(TEXT("Normal duplication creates a valid recipe GUID"), normal->RecipeGuid.IsValid());
        TestTrue(TEXT("Normal duplicate has its own recipe identity"), normal->RecipeGuid != source->RecipeGuid);
        TestEqual(TEXT("Duplication preserves ingredient rows"), normal->RecipeIngredients.Num(), source->RecipeIngredients.Num());
    }
    FObjectDuplicationParameters pieParameters(source, GetTransientPackage());
    pieParameters.DuplicateMode = EDuplicateMode::PIE;
    pieParameters.DestName = FName(*FGuid::NewGuid().ToString(EGuidFormats::Digits));
    auto* pie = Cast<UMounteaRecipeTemplate>(StaticDuplicateObjectEx(pieParameters));
    if (TestNotNull(TEXT("PIE duplication creates a recipe"), pie))
        TestEqual(TEXT("PIE duplication preserves recipe identity"), pie->RecipeGuid, source->RecipeGuid);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaRecipeFlatAssetReimportTest,
    "Mountea.InventorySystemEditor.RecipeFlatModel.ImportAndReimportAssets",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMounteaRecipeFlatAssetReimportTest::RunTest(const FString& Parameters)
{
    FFixture fixture; // Every item and recipe identity is freshly generated for this run.
    auto* source = fixture.Recipe();
    auto* first = DuplicateObject<UMounteaRecipeTemplate>(source, NewObject<UMounteaRecipeTemplate>(GetTransientPackage()), TEXT("SameNamedRecipe"));
    auto* second = DuplicateObject<UMounteaRecipeTemplate>(source, NewObject<UMounteaRecipeTemplate>(GetTransientPackage()), TEXT("SameNamedRecipe"));
    first->RecipeGuid = FGuid::NewGuid();
    second->RecipeGuid = FGuid::NewGuid();
    second->QuantityPerCreation = 7;
    const auto existingItems = UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates();
    const auto existingRecipes = FRecipeStatics::LoadAllExistingRecipes();
    for (const FGuid& guid : fixture.KnownGuids())
        if (UMounteaAdvancedInventoryItemTemplateEditorStatics::FindTemplateByGuid(existingItems, guid))
        { AddError(TEXT("Fresh item GUID already exists; refusing to import")); return false; }
    for (const auto* recipe : { first, second })
        if (FRecipeStatics::FindRecipeByGuid(existingRecipes, recipe->RecipeGuid))
        { AddError(TEXT("Fresh recipe GUID already exists; refusing to import")); return false; }
    TestEqual(TEXT("Recipes deliberately share their export name"), first->GetName(), second->GetName());

    FExportFiles files;
    if (!TestTrue(TEXT("Create isolated export directory"), IFileManager::Get().MakeDirectory(*files.Directory, true))) return false;
    FImportedAssets importedAssets { *this };
    const FString archivePath = files.File(TEXT("same-named-recipes.mntearecipes"));
    FString error;
    if (!TestTrue(TEXT("Export fresh canonical recipe bundle"), FRecipeStatics::ExportRecipesToFilePath({ first, second }, archivePath, error)))
    { AddError(error); return false; }
    TArray<UMounteaRecipeTemplate*> imported;
    if (!TestTrue(TEXT("Import exported recipe bundle as assets"), FRecipeStatics::ImportRecipesFromFilePath(archivePath, importedAssets.Folder, imported, error)))
    { AddError(error); return false; }
    TestEqual(TEXT("Both same-named recipes imported"), imported.Num(), 2);
    auto* importedFirst = FRecipeStatics::FindRecipeByGuid(imported, first->RecipeGuid);
    auto* importedSecond = FRecipeStatics::FindRecipeByGuid(imported, second->RecipeGuid);
    if (!TestNotNull(TEXT("First recipe identity imported"), importedFirst) || !TestNotNull(TEXT("Second recipe identity imported"), importedSecond)) return false;
    TestTrue(TEXT("Same-named recipes get distinct packages"), importedFirst->GetOutermost() != importedSecond->GetOutermost());
    const FString firstPackage = importedFirst->GetOutermost()->GetName();
    const FString secondPackage = importedSecond->GetOutermost()->GetName();
    const auto beforeAssets = importedAssets.Assets();
    TestEqual(TEXT("Exactly two recipes and two bundled item assets created"), beforeAssets.Num(), 4);
    TSet<FGuid> importedItemGuids;
    for (const FAssetData& asset : beforeAssets)
    {
        TestTrue(TEXT("Every created asset is under the isolated folder"), asset.PackageName.ToString().StartsWith(importedAssets.Folder + TEXT("/")));
        TestTrue(TEXT("Every imported asset is saved to disk"), FPaths::FileExists(FPackageName::LongPackageNameToFilename(asset.PackageName.ToString(), FPackageName::GetAssetPackageExtension())));
        if (auto* item = Cast<UMounteaInventoryItemTemplate>(asset.GetAsset())) importedItemGuids.Add(item->Guid);
    }
    TestEqual(TEXT("Shared bundled items imported once"), importedItemGuids.Num(), 2);
    TestTrue(TEXT("Ingredient GUID imported"), importedItemGuids.Contains(fixture.Ingredient->Guid));
    TestTrue(TEXT("Result GUID imported"), importedItemGuids.Contains(fixture.Result->Guid));
    TestEqual(TEXT("First output quantity imported"), importedFirst->QuantityPerCreation, 3);
    TestEqual(TEXT("Second output quantity imported"), importedSecond->QuantityPerCreation, 7);
    TestEqual(TEXT("Station imported"), importedFirst->RequiredCraftingPlace, first->RequiredCraftingPlace);
    TestEqual(TEXT("Duration imported"), importedFirst->CraftingTime, first->CraftingTime);
    TestEqual(TEXT("Duplicate requirements imported as one row"), importedFirst->RecipeIngredients.Num(), 1);
    if (importedFirst->RecipeIngredients.Num() == 1)
    {
        TestEqual(TEXT("Combined ingredient quantity imported"), importedFirst->RecipeIngredients[0]->RequiredQuantity, 5);
        auto* ingredient = importedFirst->RecipeIngredients[0]->IngredientSource.LoadSynchronous();
        if (TestNotNull(TEXT("Imported ingredient resolves"), ingredient))
        {
            TestEqual(TEXT("Imported ingredient has the original GUID"), ingredient->Guid, fixture.Ingredient->Guid);
            TestTrue(TEXT("Recipe references the imported ingredient asset"), ingredient->GetOutermost()->GetName().StartsWith(importedAssets.Folder + TEXT("/")));
        }
    }
    auto* result = importedFirst->ResultItem.LoadSynchronous();
    if (TestNotNull(TEXT("Imported result resolves"), result))
    {
        TestEqual(TEXT("Imported result has the original GUID"), result->Guid, fixture.Result->Guid);
        TestTrue(TEXT("Recipe references the imported result asset"), result->GetOutermost()->GetName().StartsWith(importedAssets.Folder + TEXT("/")));
    }

    // Re-export changed data with the same identities; reimport must update the existing packages.
    first->QuantityPerCreation = 11;
    first->CraftingTime = 5.5f;
    first->RequiredCraftingPlace = FGameplayTag();
    first->RecipeIngredients[1]->RequiredQuantity = 4;
    error.Reset();
    if (!TestTrue(TEXT("Export updated bundle"), FRecipeStatics::ExportRecipesToFilePath({ first, second }, archivePath, error)))
    { AddError(error); return false; }
    TArray<UMounteaRecipeTemplate*> reimported;
    error.Reset();
    if (!TestTrue(TEXT("Reimport updated bundle"), FRecipeStatics::ImportRecipesFromFilePath(archivePath, importedAssets.Folder, reimported, error)))
    { AddError(error); return false; }
    TestEqual(TEXT("Reimport returns both recipes"), reimported.Num(), 2);
    auto* updatedFirst = FRecipeStatics::FindRecipeByGuid(reimported, first->RecipeGuid);
    auto* updatedSecond = FRecipeStatics::FindRecipeByGuid(reimported, second->RecipeGuid);
    if (!TestNotNull(TEXT("Updated first recipe found"), updatedFirst) || !TestNotNull(TEXT("Updated second recipe found"), updatedSecond)) return false;
    TestTrue(TEXT("First recipe updated in place"), updatedFirst == importedFirst);
    TestTrue(TEXT("Second recipe updated in place"), updatedSecond == importedSecond);
    TestEqual(TEXT("First package preserved"), updatedFirst->GetOutermost()->GetName(), firstPackage);
    TestEqual(TEXT("Second package preserved"), updatedSecond->GetOutermost()->GetName(), secondPackage);
    TestEqual(TEXT("Reimport updated output quantity"), updatedFirst->QuantityPerCreation, 11);
    TestEqual(TEXT("Reimport updated duration"), updatedFirst->CraftingTime, 5.5f);
    TestFalse(TEXT("Reimport cleared removed station requirement"), updatedFirst->RequiredCraftingPlace.IsValid());
    TestEqual(TEXT("Reimport preserves one aggregated row"), updatedFirst->RecipeIngredients.Num(), 1);
    if (updatedFirst->RecipeIngredients.Num() == 1)
        TestEqual(TEXT("Reimport updates combined quantity"), updatedFirst->RecipeIngredients[0]->RequiredQuantity, 6);
    TestEqual(TEXT("Other recipe keeps its independent quantity"), updatedSecond->QuantityPerCreation, 7);
    TestEqual(TEXT("Reimport creates no duplicate recipe or item assets"), importedAssets.Assets().Num(), beforeAssets.Num());
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
