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

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaAdvancedInventoryRecipeEditorStatics.generated.h"

class UMounteaRecipeTemplate;
class UMounteaInventoryItemTemplate;

/**
 * Editor-only static helpers for importing and exporting .mntearecipe / .mntearecipes files.
 *
 * Import is atomic: all ZIP reading and validation finishes before any UAsset is created.
 * If validation fails, nothing is written.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventoryRecipeEditorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Main entry point. Accepts both a single .mntearecipe ZIP and a .mntearecipes bundle.
	 * On success, OutRecipes contains every recipe that was created or updated.
	 */
	static bool ImportRecipesFromFilePath(
		const FString& FilePath,
		const FString& TargetFolder,
		TArray<UMounteaRecipeTemplate*>& OutRecipes,
		FString& OutErrorMessage);

public:
	/** Export one recipe archive or a ZIP bundle; ingredient data is always flat. */
	static bool ExportRecipesToFilePath(const TArray<UMounteaRecipeTemplate*>& Recipes, const FString& FilePath, FString& OutErrorMessage);
	/** Validate the editable recipe before saving or exporting. */
	static bool ValidateRecipeTemplate(const UMounteaRecipeTemplate* Recipe, FString& OutErrorMessage);

public:
	// ── ZIP parsing ──────────────────────────────────────────────────────────

	/** Extract recipe.json, all items/*.json, and taxonomy/crafting-stations.json from a single .mntearecipe ZIP blob. */
	static bool ParseRecipeFromZip(
		const TArray<uint8>& ZipBytes,
		FString& OutRecipeJson,
		TArray<FString>& OutBundledItemJsons,
		FString& OutStationTaxonomyJson,
		FString& OutErrorMessage);

	/** Parse a taxonomy/crafting-stations.json array ([{ id, name, icon, tag }]) into a display-name -> gameplay tag lookup. */
	static TMap<FString, FString> ParseStationTagMap(const FString& TaxonomyJson);

	/** Same as above, but from an already-parsed JSON array (e.g. the "craftingStations" field of a workspace's taxonomy.json). */
	static TMap<FString, FString> ParseStationTagMap(const TArray<TSharedPtr<FJsonValue>>& Entries);

	// ── Validation (read-only, no writes) ────────────────────────────────────

	/**
	 * Validate the recipe JSON before any assets are created.
	 * KnownGuids covers bundled items + existing project items.
	 * result.itemRef and all ingredient ref.guids must be in KnownGuids.
	 */
	static bool ValidateRecipe(
		const TSharedPtr<FJsonObject>& RecipeJson,
		const TSet<FGuid>& KnownGuids,
		FString& OutErrorMessage);

	// ── Asset creation / serialization ───────────────────────────────────────

	/** Create a new UMounteaRecipeTemplate asset in TargetFolder. */
	static UMounteaRecipeTemplate* CreateRecipeAsset(
		const FString& TargetFolder,
		const FString& AssetName,
		FString& OutErrorMessage);

	/**
	 * Populate a recipe template from its JSON object.
	 * ItemsByGuid maps item GUIDs → already-imported UMounteaInventoryItemTemplate objects.
	 * Mandatory ingredients are instanced subclassable objects owned directly by the recipe.
	 */
	static bool DeserializeRecipeFromJson(
		const TSharedPtr<FJsonObject>& JsonObject,
		UMounteaRecipeTemplate* Recipe,
		const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
		const TMap<FString, FString>& StationTagByName,
		FString& OutErrorMessage);

	/** Save a recipe template package to disk and register it with the asset registry. */
	static bool SaveRecipeAsset(
		UMounteaRecipeTemplate* Recipe,
		const FString& PackagePath);

	// ── Helpers ───────────────────────────────────────────────────────────────

	static FGuid ExtractGuidFromRecipeJson(const FString& JsonString);
	static FString ExtractNameFromRecipeJson(const FString& JsonString);

	static TArray<UMounteaRecipeTemplate*> LoadAllExistingRecipes();
	static UMounteaRecipeTemplate* FindRecipeByGuid(
		const TArray<UMounteaRecipeTemplate*>& Recipes,
		const FGuid& Guid);

	static bool UpdateExistingRecipe(
		UMounteaRecipeTemplate* Recipe,
		const TSharedPtr<FJsonObject>& JsonObject,
		const TMap<FGuid, UMounteaInventoryItemTemplate*>& ItemsByGuid,
		const TMap<FString, FString>& StationTagByName,
		const FString& SourceFilePath,
		FString& OutErrorMessage);
};
