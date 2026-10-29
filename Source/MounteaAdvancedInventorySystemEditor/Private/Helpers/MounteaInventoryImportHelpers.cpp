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

#include "Helpers/MounteaInventoryImportHelpers.h"

#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Definitions/MounteaInventoryItemTemplate.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/SavePackage.h"

// ---------------------------------------------------------------------------
// JSON helpers
// ---------------------------------------------------------------------------

FString FMounteaInventoryImportHelpers::SanitizeAssetName(const FString& RawName)
{
	FString name = RawName;
	name.RemoveSpacesInline();

	const FRegexPattern invalidChars(TEXT("[\\\\:\\*\\?\"<>\\| ,.&!~@#']"));
	FRegexMatcher matcher(invalidChars, name);

	FString sanitized;
	sanitized.Reserve(name.Len());
	int32 last = 0;

	while (matcher.FindNext())
	{
		sanitized += name.Mid(last, matcher.GetMatchBeginning() - last);
		last = matcher.GetMatchEnding();
	}

	sanitized += name.Mid(last);
	return sanitized;
}

FGuid FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(const FString& JsonString, const FString& FieldName)
{
	TSharedPtr<FJsonObject> obj;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(reader, obj) || !obj.IsValid())
		return FGuid();

	FString guidStr;
	FGuid guid;

	if (obj->TryGetStringField(FieldName, guidStr) && FGuid::Parse(guidStr, guid))
		return guid;

	return FGuid();
}

bool FMounteaInventoryImportHelpers::CheckGuidInKnownSet(const FString& GuidString, const FString& Context,
	const TSet<FGuid>& KnownGuids, FString& OutError)
{
	FGuid guid;

	if (!FGuid::Parse(GuidString, guid) || !guid.IsValid())
	{
		OutError = FString::Printf(TEXT("Invalid GUID '%s' in %s"), *GuidString, *Context);
		return false;
	}

	if (!KnownGuids.Contains(guid))
	{
		OutError = FString::Printf(
			TEXT("Item GUID %s referenced in %s not found in bundled items or project"),
			*GuidString, *Context);
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// Asset system helpers
// ---------------------------------------------------------------------------

TMap<FGuid, UMounteaInventoryItemTemplate*> FMounteaInventoryImportHelpers::BuildProjectItemGuidMap()
{
	TMap<FGuid, UMounteaInventoryItemTemplate*> map;

	for (UMounteaInventoryItemTemplate* t : UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates())
	{
		if (IsValid(t))
			map.Add(t->Guid, t);
	}

	return map;
}

TSet<FGuid> FMounteaInventoryImportHelpers::ParseBundledGuids(const TArray<FString>& BundledItemJsons)
{
	TSet<FGuid> guids;

	for (const FString& json : BundledItemJsons)
	{
		const FGuid guid = UMounteaAdvancedInventoryItemTemplateEditorStatics::ExtractGuidFromJson(json);

		if (guid.IsValid())
			guids.Add(guid);
	}

	return guids;
}

bool FMounteaInventoryImportHelpers::SerializeBundledItems(const TArray<UMounteaInventoryItemTemplate*>& Items,
	TArray<FString>& OutJsons, FString& OutError)
{
	OutError.Reset();
	TMap<FGuid, UMounteaInventoryItemTemplate*> seen;
	TArray<FString> jsons;

	for (UMounteaInventoryItemTemplate* item : Items)
	{
		if (!IsValid(item) || !item->Guid.IsValid())
		{
			OutError = TEXT("Invalid bundled item or GUID");
			return false;
		}

		if (UMounteaInventoryItemTemplate** previous = seen.Find(item->Guid))
		{
			if (*previous != item)
			{
				OutError = TEXT("Different item assets share the same GUID");
				return false;
			}

			continue;
		}

		if (!item->CalculateJson())
		{
			OutError = TEXT("Failed to serialize item");
			return false;
		}

		const FString json = item->GetJson();

		if (ExtractGuidFromJsonField(json, TEXT("guid")) != item->Guid)
		{
			OutError = TEXT("Serialized item JSON has an invalid GUID");
			return false;
		}

		seen.Add(item->Guid, item);
		jsons.Add(json);
	}

	OutJsons = MoveTemp(jsons);
	return true;
}

bool FMounteaInventoryImportHelpers::ImportBundledItems(const TArray<FString>& BundledItemJsons,
	const FString& TargetFolder, const FString& SourceFilePath,
	TMap<FGuid, UMounteaInventoryItemTemplate*>& InOutItemsByGuid, FString& OutError)
{
	OutError.Reset();
	// Validate every dependency before creating packages or saving assets.
	TSet<FGuid> seen;

	for (const FString& json : BundledItemJsons)
	{
		const FGuid guid = ExtractGuidFromJsonField(json, TEXT("guid"));

		if (!guid.IsValid() || seen.Contains(guid))
		{
			OutError = TEXT("Invalid or duplicate bundled item GUID");
			return false;
		}

		UMounteaInventoryItemTemplate* transient = NewObject<UMounteaInventoryItemTemplate>();

		if (!UMounteaAdvancedInventoryItemTemplateEditorStatics::ParseSingleTemplateJson(json, transient, OutError))
			return false;

		seen.Add(guid);
	}

	const TArray<UMounteaInventoryItemTemplate*> existingTemplates = UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates();

	for (const FString& json : BundledItemJsons)
	{
		const FGuid guid = ExtractGuidFromJsonField(json, TEXT("guid"));

		if (InOutItemsByGuid.Contains(guid))
			continue;

		if (UMounteaInventoryItemTemplate* found = UMounteaAdvancedInventoryItemTemplateEditorStatics::FindTemplateByGuid(existingTemplates, guid))
		{
			InOutItemsByGuid.Add(guid, found);
			continue;
		}

		const FString displayName = UMounteaAdvancedInventoryItemTemplateEditorStatics::ExtractNameFromJson(json);
		UMounteaInventoryItemTemplate* item = UMounteaAdvancedInventoryItemTemplateEditorStatics::CreateTemplateAsset(
			TargetFolder, displayName.IsEmpty() ? guid.ToString(EGuidFormats::DigitsWithHyphens) : displayName, OutError);
		if (!item || !UMounteaAdvancedInventoryItemTemplateEditorStatics::ParseSingleTemplateJson(json, item, OutError))
			return false;

		item->ReloadItemActions();

		if (!item->CalculateJson())
		{
			OutError = TEXT("Failed to serialize imported dependency");
			return false;
		}

		item->SetSourceFilePath(SourceFilePath);

		if (!UMounteaAdvancedInventoryItemTemplateEditorStatics::SaveTemplateAsset(item, item->GetOutermost()->GetName()))
		{
			OutError = TEXT("Failed to save bundled item");
			return false;
		}

		InOutItemsByGuid.Add(guid, item);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Generic save operations
// ---------------------------------------------------------------------------

bool FMounteaInventoryImportHelpers::SaveNewAssetPackage(UObject* Asset, const FString& PackagePath)
{
	if (!IsValid(Asset))
		return false;

	UPackage* pkg = Asset->GetOutermost();
	pkg->FullyLoad();
	pkg->MarkPackageDirty();

	FSavePackageArgs saveArgs;
	saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	saveArgs.SaveFlags     = SAVE_NoError;

	const FString fileName = FPackageName::LongPackageNameToFilename(
		PackagePath, FPackageName::GetAssetPackageExtension());

	if (!UPackage::SavePackage(pkg, Asset, *fileName, saveArgs))
		return false;

	FAssetRegistryModule::AssetCreated(Asset);
	return true;
}

bool FMounteaInventoryImportHelpers::SaveExistingAssetPackage(UObject* Asset, FString& OutError)
{
	if (!IsValid(Asset))
	{
		OutError = TEXT("Invalid asset object");
		return false;
	}

	UPackage* pkg = Asset->GetOutermost();

	if (!pkg)
	{
		OutError = TEXT("Asset has no valid package");
		return false;
	}

	pkg->MarkPackageDirty();

	FSavePackageArgs saveArgs;
	saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	saveArgs.SaveFlags     = SAVE_NoError;

	const FString fileName = FPackageName::LongPackageNameToFilename(
		pkg->GetName(), FPackageName::GetAssetPackageExtension());

	if (!UPackage::SavePackage(pkg, Asset, *fileName, saveArgs))
	{
		OutError = TEXT("Failed to save updated asset");
		return false;
	}

	return true;
}
