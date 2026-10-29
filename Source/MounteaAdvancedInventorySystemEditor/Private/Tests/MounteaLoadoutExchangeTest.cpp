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
#include "AssetRegistry/AssetRegistryModule.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Definitions/MounteaInventoryBaseEnums.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "ObjectTools.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Statics/MounteaAdvancedInventoryLoadoutEditorStatics.h"
#include "UObject/Package.h"

namespace MounteaLoadoutExchangeTest
{
    using FStatics = UMounteaAdvancedInventoryLoadoutEditorStatics;
    struct FFixture
    {
        UMounteaInventoryItemTemplate* Template = NewObject<UMounteaInventoryItemTemplate>();
        FFixture()
        {
            Template->Guid = FGuid::NewGuid();
            Template->DisplayName = FText::FromString(TEXT("Loadout exchange test item"));
            Template->ItemFlags = static_cast<uint8>(EInventoryItemFlags::EIIF_Durable);
            Template->bHasDurability = true;
            Template->MaxDurability = 100;
        }
        UMounteaAdvancedInventoryLoadoutConfig* Loadout() const
        {
            auto* loadout = NewObject<UMounteaAdvancedInventoryLoadoutConfig>();
            loadout->Guid = FGuid::NewGuid();
            for (int32 index = 0; index < 2; ++index)
            {
                auto* row = NewObject<UMounteaAdvancedInventoryLoadoutItem>(loadout);
                row->Guid = Template->Guid;
                row->ItemTemplate = Template;
                row->BaseQuantity = index + 2;
                row->BaseDurability = index == 0 ? 25 : 75;
                row->bUseRandomQuantity = index == 0;
                row->RandomRange = FIntPoint(2, 9);
                row->bAutomaticallyEquip = true;
                row->EquipmentSlot = index == 0 ? TEXT("LeftHand") : TEXT("RightHand");
                loadout->Items.Add(row);
            }
            return loadout;
        }
        TSharedPtr<FJsonObject> Json() const
        {
            auto json = MakeShared<FJsonObject>();
            json->SetStringField(TEXT("guid"), FGuid::NewGuid().ToString());
            TArray<TSharedPtr<FJsonValue>> rows;
            for (int32 index = 0; index < 2; ++index)
            {
                auto row = MakeShared<FJsonObject>();
                auto ref = MakeShared<FJsonObject>();
                ref->SetStringField(TEXT("guid"), Template->Guid.ToString());
                row->SetObjectField(TEXT("ref"), ref);
                row->SetNumberField(TEXT("qty"), index + 2);
                row->SetBoolField(TEXT("autoEquip"), true);
                row->SetStringField(TEXT("slot"), index == 0 ? TEXT("LeftHand") : TEXT("RightHand"));
                rows.Add(MakeShared<FJsonValueObject>(row));
            }
            json->SetArrayField(TEXT("items"), rows);
            return json;
        }
    };
    struct FFiles
    {
        const FString Directory = FPaths::ProjectSavedDir() / TEXT("Automation/MounteaLoadoutExchange") / FGuid::NewGuid().ToString(EGuidFormats::Digits);
        TArray<FString> Paths;
        FString Path(const TCHAR* Name) { const FString path = Directory / Name; Paths.Add(path); return path; }
        ~FFiles() { for (const auto& path : Paths) IFileManager::Get().Delete(*path); IFileManager::Get().DeleteDirectory(*Directory); }
    };
    bool Check(FAutomationTestBase& Test, const UMounteaAdvancedInventoryLoadoutConfig* Actual,
        const UMounteaAdvancedInventoryLoadoutConfig* Expected)
    {
        Test.TestEqual(TEXT("Loadout GUID retained"), Actual->Guid, Expected->Guid);
        if (!Test.TestEqual(TEXT("Independent duplicate-template rows retained"), Actual->Items.Num(), 2)) return false;
        for (int32 index = 0; index < 2; ++index)
        {
            const auto* row = Actual->Items[index].Get();
            const auto* expected = Expected->Items[index].Get();
            Test.TestEqual(TEXT("Template GUID retained"), row->ItemTemplate->Guid, expected->ItemTemplate->Guid);
            Test.TestEqual(TEXT("Row GUID matches template"), row->Guid, row->ItemTemplate->Guid);
            Test.TestEqual(TEXT("Per-row quantity retained"), row->BaseQuantity, expected->BaseQuantity);
            Test.TestEqual(TEXT("Durability fraction restores absolute durability"), row->BaseDurability, expected->BaseDurability);
            Test.TestEqual(TEXT("Random quantity flag retained"), bool(row->bUseRandomQuantity), bool(expected->bUseRandomQuantity));
            Test.TestEqual(TEXT("Random min retained"), row->RandomRange.X, expected->RandomRange.X);
            Test.TestEqual(TEXT("Random max retained"), row->RandomRange.Y, expected->RandomRange.Y);
            Test.TestEqual(TEXT("Auto equip retained"), bool(row->bAutomaticallyEquip), bool(expected->bAutomaticallyEquip));
            Test.TestEqual(TEXT("Per-row slot retained"), row->EquipmentSlot, expected->EquipmentSlot);
        }
        Test.TestTrue(TEXT("Rows remain distinct instances"), Actual->Items[0] != Actual->Items[1]);
        return true;
    }
}
using namespace MounteaLoadoutExchangeTest;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaLoadoutExportRoundTripTest,
    "Mountea.InventorySystemEditor.LoadoutExchange.SingleAndBundleRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FMounteaLoadoutExportRoundTripTest::RunTest(const FString& Parameters)
{
    auto* native = NewObject<UMounteaAdvancedInventoryLoadoutConfig>();
    TestTrue(TEXT("New native loadouts have an exportable GUID"), native->Guid.IsValid());
    auto* duplicated = DuplicateObject<UMounteaAdvancedInventoryLoadoutConfig>(native, GetTransientPackage());
    TestTrue(TEXT("Duplicated native loadouts receive a distinct GUID"), duplicated->Guid.IsValid() && duplicated->Guid != native->Guid);
    FFixture fixture;
    auto* first = fixture.Loadout(); auto* second = fixture.Loadout();
    second->Items[1]->BaseQuantity = 8;
    FFiles files;
    if (!TestTrue(TEXT("Create isolated Saved directory"), IFileManager::Get().MakeDirectory(*files.Directory, true))) return false;
    for (const bool bundle : { false, true })
    {
        const FString path = files.Path(bundle ? TEXT("bundle.mntealoadouts") : TEXT("single.mntealoadout"));
        const TArray<UMounteaAdvancedInventoryLoadoutConfig*> selection = bundle ? TArray<UMounteaAdvancedInventoryLoadoutConfig*> { first, second } : TArray<UMounteaAdvancedInventoryLoadoutConfig*> { first };
        FString error;
        if (!TestTrue(TEXT("Export loadout selection"), FStatics::ExportLoadoutsToFilePath(selection, path, error))) { AddError(error); return false; }
        TArray<TArray<uint8>> archives;
        if (!TestTrue(TEXT("Read common exchange contract"), FMounteaInventoryZipHelper::ReadEntityArchives(path, TEXT("mntealoadout"), TEXT("mntealoadouts"), archives, error))) return false;
        TestEqual(TEXT("Archive selection count"), archives.Num(), selection.Num());
        TSet<FGuid> seen;
        for (const auto& archive : archives)
        {
            FString jsonText; TArray<FString> items;
            if (!TestTrue(TEXT("Parse loadout archive"), FStatics::ParseLoadoutFromZip(archive, jsonText, items, error))) return false;
            TestEqual(TEXT("Shared template bundled once"), items.Num(), 1);
            TSharedPtr<FJsonObject> json;
            if (!TestTrue(TEXT("Loadout JSON parses"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(jsonText), json))) return false;
            TestFalse(TEXT("Export has no root slots representation"), json->HasField(TEXT("slots")));
            TestEqual(TEXT("Export stores durability as fraction"), json->GetArrayField(TEXT("items"))[0]->AsObject()->GetNumberField(TEXT("durability")), 0.25);
            auto* restored = NewObject<UMounteaAdvancedInventoryLoadoutConfig>();
            if (!TestTrue(TEXT("Deserialize native loadout"), FStatics::DeserializeLoadoutFromJson(json, restored, { { fixture.Template->Guid, fixture.Template } }, error))) return false;
            auto* expected = restored->Guid == first->Guid ? first : second;
            TestTrue(TEXT("Archive identity belongs to selection"), selection.Contains(expected) && restored->Guid == expected->Guid);
            seen.Add(restored->Guid);
            Check(*this, restored, expected);
            TestTrue(TEXT("Template pointer resolved from GUID map"), restored->Items[0]->ItemTemplate.Get() == fixture.Template);
        }
        TestEqual(TEXT("Every selection has distinct identity"), seen.Num(), selection.Num());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaLoadoutValidationTest,
    "Mountea.InventorySystemEditor.LoadoutExchange.RejectBeforeMutation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FMounteaLoadoutValidationTest::RunTest(const FString& Parameters)
{
    FFixture fixture; auto* unchanged = fixture.Loadout();
    const FGuid originalGuid = unchanged->Guid; auto* originalRow = unchanged->Items[0].Get();
    const auto reject = [&](const TSharedPtr<FJsonObject>& json)
    {
        FString error;
        TestFalse(TEXT("Invalid JSON rejects"), FStatics::ValidateLoadout(json, { fixture.Template->Guid }, error));
        TestFalse(TEXT("Invalid JSON has an explanation"), error.IsEmpty());
        TestFalse(TEXT("Invalid deserialize rejects"), FStatics::DeserializeLoadoutFromJson(json, unchanged, { { fixture.Template->Guid, fixture.Template } }, error));
        TestEqual(TEXT("Failure preserves GUID"), unchanged->Guid, originalGuid);
        TestEqual(TEXT("Failure preserves rows"), unchanged->Items.Num(), 2);
        TestTrue(TEXT("Failure preserves row instance"), unchanged->Items[0].Get() == originalRow);
        TestEqual(TEXT("Failure preserves quantity"), unchanged->Items[0]->BaseQuantity, 2);
        TestEqual(TEXT("Failure preserves durability"), unchanged->Items[0]->BaseDurability, 25);
    };
    { auto json = fixture.Json(); json->SetObjectField(TEXT("slots"), MakeShared<FJsonObject>()); reject(json); }
    { auto json = fixture.Json(); json->GetArrayField(TEXT("items"))[1]->AsObject()->SetStringField(TEXT("slot"), TEXT("lefthand")); reject(json); }
    for (const double quantity : { 0.0, 1.5 })
    { auto json = fixture.Json(); json->GetArrayField(TEXT("items"))[1]->AsObject()->SetNumberField(TEXT("qty"), quantity); reject(json); }
    for (const TCHAR* guid : { TEXT("invalid"), TEXT("00000000-0000-0000-0000-000000000000") })
    { auto json = fixture.Json(); json->SetStringField(TEXT("guid"), guid); reject(json); }
    { auto json = fixture.Json(); json->GetArrayField(TEXT("items"))[1]->AsObject()->GetObjectField(TEXT("ref"))->SetStringField(TEXT("guid"), FGuid::NewGuid().ToString()); reject(json); }
    { auto json = fixture.Json(); json->RemoveField(TEXT("items")); reject(json); }
    FString error; auto empty = fixture.Json(); empty->SetArrayField(TEXT("items"), {});
    TestTrue(TEXT("Explicit empty loadout is valid"), FStatics::DeserializeLoadoutFromJson(empty, unchanged, {}, error));
    TestTrue(TEXT("Empty loadout clears old rows"), unchanged->Items.IsEmpty());
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaLoadoutAssetReimportTest,
    "Mountea.InventorySystemEditor.LoadoutExchange.ImportAndReimportAssets",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FMounteaLoadoutAssetReimportTest::RunTest(const FString& Parameters)
{
    FFixture fixture; auto* source = fixture.Loadout();
    const auto existingItems = UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates();
    if (UMounteaAdvancedInventoryItemTemplateEditorStatics::FindTemplateByGuid(existingItems, fixture.Template->Guid) ||
        FStatics::FindLoadoutByGuid(FStatics::LoadAllExistingLoadouts(), source->Guid))
    { AddError(TEXT("Fresh GUID already exists; refusing asset import")); return false; }
    struct FImportedAssets
    {
        FAutomationTestBase& Test;
        const FString Folder = TEXT("/Game/MounteaInventoryTests/Import/") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
        TArray<FAssetData> Assets() const
        {
            auto& registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
            registry.ScanPathsSynchronous({ Folder }, true);
            TArray<FAssetData> assets; registry.GetAssetsByPath(FName(*Folder), assets, true); return assets;
        }
        ~FImportedAssets()
        {
            const FString root = TEXT("/Game/MounteaInventoryTests/Import/");
            if (!Folder.StartsWith(root) || Folder.Len() != root.Len() + 32)
            { Test.AddError(TEXT("Refusing cleanup outside isolated loadout fixture")); return; }
            const auto assets = Assets();
            for (const auto& asset : assets)
                if (!asset.PackageName.ToString().StartsWith(Folder + TEXT("/")))
                { Test.AddError(TEXT("Refusing cleanup of unrelated asset")); return; }
            if (!assets.IsEmpty()) ObjectTools::DeleteAssets(assets, false);
            const FString directory = FPackageName::LongPackageNameToFilename(Folder);
            if (FPaths::DirectoryExists(directory) && !IFileManager::Get().DeleteDirectory(*directory, false, true))
                Test.AddError(TEXT("Could not remove isolated loadout directory"));
            if (!Assets().IsEmpty()) Test.AddError(TEXT("Could not remove isolated loadout assets"));
        }
    } importedAssets { *this };
    FFiles files;
    if (!TestTrue(TEXT("Create isolated export folder"), IFileManager::Get().MakeDirectory(*files.Directory, true))) return false;
    const FString path = files.Path(TEXT("import.mntealoadout"));
    FString error;
    if (!TestTrue(TEXT("Export fresh native loadout"), FStatics::ExportLoadoutsToFilePath({ source }, path, error))) { AddError(error); return false; }
    TArray<UMounteaAdvancedInventoryLoadoutConfig*> imported;
    if (!TestTrue(TEXT("Import as real assets"), FStatics::ImportLoadoutsFromFilePath(path, importedAssets.Folder, imported, error))) { AddError(error); return false; }
    if (!TestEqual(TEXT("One loadout imported"), imported.Num(), 1)) return false;
    Check(*this, imported[0], source);
    TestEqual(TEXT("Loadout and shared dependency created once"), importedAssets.Assets().Num(), 2);
    for (const auto& asset : importedAssets.Assets())
        TestTrue(TEXT("Imported package saved"), FPaths::FileExists(FPackageName::LongPackageNameToFilename(asset.PackageName.ToString(), FPackageName::GetAssetPackageExtension())));
    const FString package = imported[0]->GetOutermost()->GetName();
    TestTrue(TEXT("Loadout resides in isolated folder"), package.StartsWith(importedAssets.Folder + TEXT("/")));
    TestTrue(TEXT("Dependency resolves to imported asset"), imported[0]->Items[0]->ItemTemplate->GetOutermost()->GetName().StartsWith(importedAssets.Folder + TEXT("/")));
    source->Items[0]->BaseQuantity = 11;
    source->Items[0]->BaseDurability = 50;
    if (!TestTrue(TEXT("Export changed same identity"), FStatics::ExportLoadoutsToFilePath({ source }, path, error))) { AddError(error); return false; }
    TArray<UMounteaAdvancedInventoryLoadoutConfig*> reimported;
    if (!TestTrue(TEXT("Reimport existing identity"), FStatics::ImportLoadoutsFromFilePath(path, importedAssets.Folder, reimported, error))) { AddError(error); return false; }
    if (!TestEqual(TEXT("One loadout reimported"), reimported.Num(), 1)) return false;
    TestTrue(TEXT("Reimport updates same object"), reimported[0] == imported[0]);
    TestEqual(TEXT("Reimport retains package"), reimported[0]->GetOutermost()->GetName(), package);
    Check(*this, reimported[0], source);
    TestEqual(TEXT("Reimport creates no duplicate assets"), importedAssets.Assets().Num(), 2);
    return true;
}
#endif
