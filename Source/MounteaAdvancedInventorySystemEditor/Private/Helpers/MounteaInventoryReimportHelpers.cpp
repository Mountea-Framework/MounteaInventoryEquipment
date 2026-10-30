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

#include "Helpers/MounteaInventoryReimportHelpers.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Dom/JsonObject.h"
#include "Helpers/MounteaInventoryImportHelpers.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryReimportHelpers"

namespace
{
	bool AddRecord(const FString& Json, TMap<FGuid, FString>& Records, FString& Error)
	{
		const FGuid guid = FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(Json, TEXT("guid"));

		if (!guid.IsValid())
		{
			Error = TEXT("Source contains an invalid entity GUID.");
			return false;
		}

		if (const FString* existing = Records.Find(guid))
		{
			if (*existing != Json)
			{
				Error = TEXT("Source contains conflicting definitions for the same GUID.");
				return false;
			}
		}

		Records.Add(guid, Json);
		return true;
	}

	FString SourcePath(UObject* Asset)
	{
		if (const auto* item = Cast<UMounteaInventoryItemTemplate>(Asset))
			return item->GetSourceFilePath();

		if (const auto* recipe = Cast<UMounteaRecipeTemplate>(Asset))
			return recipe->GetSourceFilePath();

		return FString();
	}
}

bool FMounteaInventoryReimportHelpers::ReadSource(const FString& FilePath, FMounteaInventoryReimportSource& OutSource)
{
	OutSource = FMounteaInventoryReimportSource();
	const FString extension = FPaths::GetExtension(FilePath).ToLower();

	if (extension == TEXT("mnteainventory"))
	{
		TArray<uint8> bytes;
		TMap<FString, FString> textFiles;
		TMap<FString, TArray<uint8>> binaryFiles;

		if (!FMounteaInventoryZipHelper::LoadFileToBytes(FilePath, bytes) ||
			!FMounteaInventoryZipHelper::ExtractEntries(bytes, textFiles, binaryFiles))
		{
			OutSource.Error = TEXT("Cannot read workspace ZIP source.");
			return false;
		}

		TMap<FString, FString> stations;

		if (const FString* taxonomy = textFiles.Find(TEXT("taxonomy.json")))
		{
			TSharedPtr<FJsonObject> object;

			if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(*taxonomy), object) || !object.IsValid())
			{
				OutSource.Error = TEXT("Invalid workspace taxonomy.");
				return false;
			}

			const TArray<TSharedPtr<FJsonValue>>* entries = nullptr;

			if (object->TryGetArrayField(TEXT("craftingStations"), entries))
				stations = UMounteaAdvancedInventoryRecipeEditorStatics::ParseStationTagMap(*entries);
		}

		for (const auto& entry : textFiles)
		{
			const bool isItem = entry.Key.StartsWith(TEXT("items/"));
			const bool isRecipe = entry.Key.StartsWith(TEXT("crafting/"));

			if ((!isItem && !isRecipe) || !entry.Key.EndsWith(TEXT(".json")))
				continue;

			const FGuid guid = FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(entry.Value, TEXT("guid"));
			FGuid fileGuid;

			if (!FGuid::ParseExact(FPaths::GetBaseFilename(entry.Key), EGuidFormats::DigitsWithHyphens, fileGuid) ||
				fileGuid != guid || FPaths::GetPath(entry.Key) != (isItem ? TEXT("items") : TEXT("crafting")))
			{
				OutSource.Error = TEXT("Workspace entity path does not match its GUID.");
				return false;
			}

			if (!AddRecord(entry.Value, isItem ? OutSource.Items : OutSource.Recipes, OutSource.Error))
				return false;

			if (isRecipe)
				OutSource.RecipeStations.Add(guid, stations);
		}

		return true;
	}

	FString singleExtension;
	FString rootFile;

	if (extension == TEXT("mnteaitem") || extension == TEXT("mnteaitems"))
	{
		singleExtension = TEXT("mnteaitem");
		rootFile = TEXT("item.json");
	}
	else if (extension == TEXT("mntearecipe") || extension == TEXT("mntearecipes"))
	{
		singleExtension = TEXT("mntearecipe");
		rootFile = TEXT("recipe.json");
	}
	else if (extension == TEXT("mntealoadout") || extension == TEXT("mntealoadouts"))
	{
		singleExtension = TEXT("mntealoadout");
		rootFile = TEXT("loadout.json");
	}
	else
	{
		OutSource.Error = TEXT("Unsupported source file extension.");
		return false;
	}

	TArray<TArray<uint8>> archives;

	if (!FMounteaInventoryZipHelper::ReadEntityArchives(FilePath, singleExtension, singleExtension + TEXT("s"), archives, OutSource.Error))
		return false;

	for (const TArray<uint8>& archive : archives)
	{
		FString json;
		FString taxonomy;
		TArray<FString> items;

		if (!FMounteaInventoryZipHelper::ExtractEntityArchive(archive, rootFile, json, items, taxonomy, OutSource.Error))
			return false;

		if (rootFile == TEXT("item.json"))
		{
			if (!AddRecord(json, OutSource.Items, OutSource.Error))
				return false;
		}
		else if (rootFile == TEXT("recipe.json"))
		{
			if (!AddRecord(json, OutSource.Recipes, OutSource.Error))
				return false;

			const FGuid guid = FMounteaInventoryImportHelpers::ExtractGuidFromJsonField(json, TEXT("guid"));
			OutSource.RecipeStations.Add(guid, UMounteaAdvancedInventoryRecipeEditorStatics::ParseStationTagMap(taxonomy));
		}

		for (const FString& item : items)
		{
			if (!AddRecord(item, OutSource.Items, OutSource.Error))
				return false;
		}
	}

	return true;
}

FMounteaInventoryReimportResult FMounteaInventoryReimportHelpers::Reimport(const TArray<UObject*>& Assets)
{
	FMounteaInventoryReimportResult result;
	TArray<UObject*> candidates;
	int32 dirtyCount = 0;

	for (UObject* asset : Assets)
	{
		if (!IsValid(asset) || asset->HasAnyFlags(RF_Transient) || asset->GetOutermost() == GetTransientPackage())
			continue;

		if (SourcePath(asset).IsEmpty())
		{
			++result.Skipped;
			continue;
		}

		candidates.AddUnique(asset);

		if (asset->GetOutermost()->IsDirty())
			++dirtyCount;
	}

	if (dirtyCount > 0 && FMessageDialog::Open(EAppMsgType::OkCancel,
		FText::Format(LOCTEXT("OverwriteDirty", "Reimport All will overwrite and save {0} modified source-backed assets. Continue? Unsaved drafts are preserved."),
			FText::AsNumber(dirtyCount))) != EAppReturnType::Ok)
	{
		result.bCancelled = true;
		return result;
	}

	TMap<FString, FMounteaInventoryReimportSource> sources;
	const TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = FMounteaInventoryImportHelpers::BuildProjectItemGuidMap();

	for (UObject* asset : candidates)
	{
		const FString recordedPath = SourcePath(asset);
		FString path = FPaths::ConvertRelativePathToFull(recordedPath);
		FPaths::NormalizeFilename(path);
		FMounteaInventoryReimportSource* source = sources.Find(path);

		if (!source)
		{
			source = &sources.Add(path);
			ReadSource(path, *source);
		}

		FString error = source->Error;
		bool updated = false;

		if (error.IsEmpty())
		{
			if (auto* item = Cast<UMounteaInventoryItemTemplate>(asset))
			{
				if (const FString* json = source->Items.Find(item->Guid))
					updated = UMounteaAdvancedInventoryItemTemplateEditorStatics::UpdateExistingTemplate(item, *json, recordedPath, error);
				else
					error = TEXT("Item GUID was not found in the recorded source.");
			}
			else if (auto* recipe = Cast<UMounteaRecipeTemplate>(asset))
			{
				if (const FString* json = source->Recipes.Find(recipe->RecipeGuid))
				{
					TSharedPtr<FJsonObject> object;

					if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(*json), object) && object.IsValid())
						updated = UMounteaAdvancedInventoryRecipeEditorStatics::UpdateExistingRecipe(recipe, object, itemsByGuid,
							source->RecipeStations.FindChecked(recipe->RecipeGuid), recordedPath, error);
					else
						error = TEXT("Invalid recipe JSON.");
				}
				else
					error = TEXT("Recipe GUID was not found in the recorded source.");
			}
		}

		if (updated)
			result.Updated.Add(asset);
		else
			result.Failures.Add(FString::Printf(TEXT("%s (%s): %s"), *asset->GetPathName(), *recordedPath, *error));
	}

	return result;
}

void FMounteaInventoryReimportHelpers::ShowResult(const FMounteaInventoryReimportResult& Result)
{
	if (Result.bCancelled)
		return;

	FString summary = FString::Printf(TEXT("Reimported: %d\nSkipped (no recorded source): %d\nFailed: %d"),
		Result.Updated.Num(), Result.Skipped, Result.Failures.Num());

	if (!Result.Failures.IsEmpty())
		summary += TEXT("\n\n") + FString::Join(Result.Failures, TEXT("\n"));

	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(summary), LOCTEXT("Summary", "Reimport All"));
}

#undef LOCTEXT_NAMESPACE
