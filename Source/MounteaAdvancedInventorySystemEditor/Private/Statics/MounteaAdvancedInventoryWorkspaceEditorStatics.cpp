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

#include "Statics/MounteaAdvancedInventoryWorkspaceEditorStatics.h"

#include "Helpers/MounteaInventoryImportHelpers.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Statics/MounteaAdvancedInventoryLoadoutEditorStatics.h"
#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

#include "Dom/JsonObject.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool UMounteaAdvancedInventoryWorkspaceEditorStatics::ImportWorkspaceFromFile(
	TArray<UMounteaInventoryItemTemplate*>& OutItems,
	TArray<UMounteaRecipeTemplate*>& OutRecipes,
	TArray<UMounteaAdvancedInventoryLoadoutConfig*>& OutLoadouts,
	FString& OutErrorMessage)
{
	const FString filePath = UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowOpenFileDialog(
		TEXT("Import Workspace"),
		TEXT("Mountea Workspace File (*.mnteainventory)|*.mnteainventory")
	);

	if (filePath.IsEmpty())
	{
		OutErrorMessage = TEXT("No file selected");
		return false;
	}

	const FString targetFolder = UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowContentBrowserPathPicker(
		TEXT("Select Target Root Folder for Workspace Import"),
		TEXT("/Game/")
	);

	if (targetFolder.IsEmpty())
	{
		OutErrorMessage = TEXT("No target folder selected");
		return false;
	}

	return ImportWorkspaceFromFilePath(filePath, targetFolder, OutItems, OutRecipes, OutLoadouts, OutErrorMessage);
}

bool UMounteaAdvancedInventoryWorkspaceEditorStatics::ImportWorkspaceFromFilePath(
	const FString& FilePath,
	const FString& TargetRootFolder,
	TArray<UMounteaInventoryItemTemplate*>& OutItems,
	TArray<UMounteaRecipeTemplate*>& OutRecipes,
	TArray<UMounteaAdvancedInventoryLoadoutConfig*>& OutLoadouts,
	FString& OutErrorMessage)
{
	TArray<uint8> fileBytes;
	if (!FMounteaInventoryZipHelper::LoadFileToBytes(FilePath, fileBytes))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
		LOG_ERROR(TEXT("[Workspace Import] %s"), *OutErrorMessage);
		return false;
	}

	if (!FMounteaInventoryZipHelper::IsZipFile(fileBytes))
	{
		OutErrorMessage = TEXT("File is not a valid ZIP archive");
		LOG_ERROR(TEXT("[Workspace Import] %s"), *OutErrorMessage);
		return false;
	}

	TMap<FString, FString> textFiles;
	TMap<FString, TArray<uint8>> binaryFiles;
	if (!FMounteaInventoryZipHelper::ExtractEntries(fileBytes, textFiles, binaryFiles))
	{
		OutErrorMessage = TEXT("Failed to extract ZIP entries");
		LOG_ERROR(TEXT("[Workspace Import] %s"), *OutErrorMessage);
		return false;
	}

	if (const FString* manifestJson = textFiles.Find(TEXT("manifest.json")))
	{
		TSharedPtr<FJsonObject> manifestObj;
		TSharedRef<TJsonReader<>> manifestReader = TJsonReaderFactory<>::Create(*manifestJson);
		if (FJsonSerializer::Deserialize(manifestReader, manifestObj) && manifestObj.IsValid())
		{
			FString schemaVersion, exportedAt;
			manifestObj->TryGetStringField(TEXT("schemaVersion"), schemaVersion);
			manifestObj->TryGetStringField(TEXT("exportedAt"), exportedAt);
			LOG_INFO(TEXT("[Workspace Import] Importing workspace (schema %s, exported %s)"), *schemaVersion, *exportedAt);
		}
	}

	// taxonomy.json's "craftingStations" array is the workspace equivalent of a per-recipe taxonomy/crafting-stations.json.
	TMap<FString, FString> stationTagByName;
	if (const FString* taxonomyJson = textFiles.Find(TEXT("taxonomy.json")))
	{
		TSharedPtr<FJsonObject> taxonomyObj;
		TSharedRef<TJsonReader<>> taxonomyReader = TJsonReaderFactory<>::Create(*taxonomyJson);
		if (FJsonSerializer::Deserialize(taxonomyReader, taxonomyObj) && taxonomyObj.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* craftingStations;
			if (taxonomyObj->TryGetArrayField(TEXT("craftingStations"), craftingStations))
				stationTagByName = UMounteaAdvancedInventoryRecipeEditorStatics::ParseStationTagMap(*craftingStations);
		}
		else
			LOG_WARNING(TEXT("[Workspace Import] Failed to parse taxonomy.json"));
	}

	const FString itemsFolder = FPaths::Combine(TargetRootFolder, TEXT("Items"));
	const FString recipesFolder = FPaths::Combine(TargetRootFolder, TEXT("Recipes"));
	const FString loadoutsFolder = FPaths::Combine(TargetRootFolder, TEXT("Loadouts"));

	// ── Phase 1: import every item first, building a workspace-wide GUID -> template map ──

	TArray<FString> itemJsons;
	for (const auto& kv : textFiles)
		if (kv.Key.StartsWith(TEXT("items/")) && kv.Key.EndsWith(TEXT(".json")))
			itemJsons.Add(kv.Value);

	TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = FMounteaInventoryImportHelpers::BuildProjectItemGuidMap();
	FMounteaInventoryImportHelpers::ImportBundledItems(itemJsons, itemsFolder, FilePath, itemsByGuid);

	for (const FString& itemJson : itemJsons)
	{
		const FGuid itemGuid = UMounteaAdvancedInventoryItemTemplateEditorStatics::ExtractGuidFromJson(itemJson);
		if (UMounteaInventoryItemTemplate* const* found = itemsByGuid.Find(itemGuid))
			OutItems.AddUnique(*found);
		else
			LOG_WARNING(TEXT("[Workspace Import] Item GUID %s could not be resolved"), *itemGuid.ToString());
	}

	TSet<FGuid> knownGuids;
	for (const auto& kv : itemsByGuid)
		knownGuids.Add(kv.Key);

	// ── Phase 2: recipes (best-effort — a malformed recipe is skipped, not fatal to the rest) ──

	TArray<UMounteaRecipeTemplate*> existingRecipes = UMounteaAdvancedInventoryRecipeEditorStatics::LoadAllExistingRecipes();

	for (const auto& kv : textFiles)
	{
		if (!kv.Key.StartsWith(TEXT("crafting/")) || !kv.Key.EndsWith(TEXT(".json")))
			continue;

		TSharedPtr<FJsonObject> recipeJson;
		TSharedRef<TJsonReader<>> recipeReader = TJsonReaderFactory<>::Create(kv.Value);
		if (!FJsonSerializer::Deserialize(recipeReader, recipeJson) || !recipeJson.IsValid())
		{
			OutErrorMessage += FString::Printf(TEXT("Failed to parse %s\n"), *kv.Key);
			LOG_ERROR(TEXT("[Workspace Import] Failed to parse %s"), *kv.Key);
			continue;
		}

		FString validationError;
		if (!UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipe(recipeJson, knownGuids, validationError))
		{
			OutErrorMessage += validationError + TEXT("\n");
			continue;
		}

		const FGuid recipeGuid = UMounteaAdvancedInventoryRecipeEditorStatics::ExtractGuidFromRecipeJson(kv.Value);
		UMounteaRecipeTemplate* recipe = UMounteaAdvancedInventoryRecipeEditorStatics::FindRecipeByGuid(existingRecipes, recipeGuid);

		if (recipe)
		{
			FString updateError;
			if (UMounteaAdvancedInventoryRecipeEditorStatics::UpdateExistingRecipe(recipe, recipeJson, itemsByGuid, stationTagByName, FilePath, updateError))
				OutRecipes.Add(recipe);
			else
				OutErrorMessage += updateError + TEXT("\n");
		}
		else
		{
			const FString name = UMounteaAdvancedInventoryRecipeEditorStatics::ExtractNameFromRecipeJson(kv.Value);
			const FString assetName = name.IsEmpty() ? recipeGuid.ToString(EGuidFormats::Short) : name;

			FString createError;
			recipe = UMounteaAdvancedInventoryRecipeEditorStatics::CreateRecipeAsset(recipesFolder, assetName, createError);
			if (!recipe)
			{
				OutErrorMessage += createError + TEXT("\n");
				continue;
			}

			FString deserError;
			if (UMounteaAdvancedInventoryRecipeEditorStatics::DeserializeRecipeFromJson(recipeJson, recipe, itemsByGuid, stationTagByName, deserError))
			{
				recipe->SetSourceFilePath(FilePath);
				if (UMounteaAdvancedInventoryRecipeEditorStatics::SaveRecipeAsset(recipe, recipe->GetOutermost()->GetName()))
					OutRecipes.Add(recipe);
				else
					OutErrorMessage += FString::Printf(TEXT("Failed to save recipe asset '%s'\n"), *assetName);
			}
			else
				OutErrorMessage += deserError + TEXT("\n");
		}
	}

	// ── Phase 3: loadouts (same best-effort treatment) ──

	TArray<UMounteaAdvancedInventoryLoadoutConfig*> existingLoadouts = UMounteaAdvancedInventoryLoadoutEditorStatics::LoadAllExistingLoadouts();

	for (const auto& kv : textFiles)
	{
		if (!kv.Key.StartsWith(TEXT("loadouts/")) || !kv.Key.EndsWith(TEXT(".json")))
			continue;

		TSharedPtr<FJsonObject> loadoutJson;
		TSharedRef<TJsonReader<>> loadoutReader = TJsonReaderFactory<>::Create(kv.Value);
		if (!FJsonSerializer::Deserialize(loadoutReader, loadoutJson) || !loadoutJson.IsValid())
		{
			OutErrorMessage += FString::Printf(TEXT("Failed to parse %s\n"), *kv.Key);
			LOG_ERROR(TEXT("[Workspace Import] Failed to parse %s"), *kv.Key);
			continue;
		}

		FString validationError;
		if (!UMounteaAdvancedInventoryLoadoutEditorStatics::ValidateLoadout(loadoutJson, knownGuids, validationError))
		{
			OutErrorMessage += validationError + TEXT("\n");
			continue;
		}

		const FGuid loadoutGuid = UMounteaAdvancedInventoryLoadoutEditorStatics::ExtractGuidFromLoadoutJson(kv.Value);
		UMounteaAdvancedInventoryLoadoutConfig* loadout = UMounteaAdvancedInventoryLoadoutEditorStatics::FindLoadoutByGuid(existingLoadouts, loadoutGuid);

		if (loadout)
		{
			FString updateError;
			if (UMounteaAdvancedInventoryLoadoutEditorStatics::UpdateExistingLoadout(loadout, loadoutJson, itemsByGuid, FilePath, updateError))
				OutLoadouts.Add(loadout);
			else
				OutErrorMessage += updateError + TEXT("\n");
		}
		else
		{
			const FString name = UMounteaAdvancedInventoryLoadoutEditorStatics::ExtractNameFromLoadoutJson(kv.Value);
			const FString assetName = name.IsEmpty() ? loadoutGuid.ToString(EGuidFormats::Short) : name;

			FString createError;
			loadout = UMounteaAdvancedInventoryLoadoutEditorStatics::CreateLoadoutAsset(loadoutsFolder, assetName, createError);
			if (!loadout)
			{
				OutErrorMessage += createError + TEXT("\n");
				continue;
			}

			FString deserError;
			if (UMounteaAdvancedInventoryLoadoutEditorStatics::DeserializeLoadoutFromJson(loadoutJson, loadout, itemsByGuid, deserError))
			{
				loadout->SetSourceFilePath(FilePath);
				if (UMounteaAdvancedInventoryLoadoutEditorStatics::SaveLoadoutAsset(loadout, FPaths::Combine(loadoutsFolder, assetName)))
					OutLoadouts.Add(loadout);
				else
					OutErrorMessage += FString::Printf(TEXT("Failed to save loadout asset '%s'\n"), *assetName);
			}
			else
				OutErrorMessage += deserError + TEXT("\n");
		}
	}

	if (OutItems.Num() == 0 && OutRecipes.Num() == 0 && OutLoadouts.Num() == 0)
	{
		OutErrorMessage = TEXT("No workspace content was successfully imported");
		LOG_ERROR(TEXT("[Workspace Import] %s"), *OutErrorMessage);
		return false;
	}

	OutErrorMessage = FString::Printf(TEXT("Workspace import complete: %d items, %d recipes, %d loadouts"),
		OutItems.Num(), OutRecipes.Num(), OutLoadouts.Num());
	return true;
}
