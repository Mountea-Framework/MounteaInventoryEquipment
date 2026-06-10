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

class UMounteaInventoryItemTemplate;

/**
 * Shared static helpers used by all three import statics (item template, loadout, recipe).
 * All methods are static; this struct is never instantiated.
 */
struct FMounteaInventoryImportHelpers
{
	// ── JSON helpers (no UE asset system) ─────────────────────────────────────

	/** Strips spaces and asset-illegal characters so the result is safe as a UE asset name. */
	static FString SanitizeAssetName(const FString& RawName);

	/** Parses JsonString and returns the FGuid stored under FieldName (e.g. TEXT("guid")). */
	static FGuid ExtractGuidFromJsonField(const FString& JsonString, const FString& FieldName);

	/**
	 * Validates GuidString against KnownGuids.
	 * Returns false and fills OutError if the string is not a valid GUID or is absent from the set.
	 */
	static bool CheckGuidInKnownSet(const FString& GuidString, const FString& Context,
		const TSet<FGuid>& KnownGuids, FString& OutError);

	// ── Asset system helpers ───────────────────────────────────────────────────

	/** Builds a GUID→template map from all UMounteaInventoryItemTemplate assets in the project. */
	static TMap<FGuid, UMounteaInventoryItemTemplate*> BuildProjectItemGuidMap();

	/** Extracts GUIDs from bundled item JSON strings (items/*.json entries from a single-entity ZIP). */
	static TSet<FGuid> ParseBundledGuids(const TArray<FString>& BundledItemJsons);

	/**
	 * For each JSON in BundledItemJsons: finds an existing template by GUID in the project
	 * or creates a new one from the JSON, then adds it to InOutItemsByGuid.
	 * Entries whose GUID is already present in InOutItemsByGuid are skipped.
	 */
	static void ImportBundledItems(const TArray<FString>& BundledItemJsons,
		const FString& TargetFolder,
		TMap<FGuid, UMounteaInventoryItemTemplate*>& InOutItemsByGuid);

	// ── Generic save operations ────────────────────────────────────────────────

	/**
	 * Saves a newly created asset to an explicit package path.
	 * Calls FullyLoad, MarkPackageDirty, SavePackage, then notifies the asset registry.
	 */
	static bool SaveNewAssetPackage(UObject* Asset, const FString& PackagePath);

	/**
	 * Saves an already-loaded asset back to its own package (no explicit path needed).
	 * Calls MarkPackageDirty, SavePackage. Does not call FullyLoad or AssetCreated.
	 */
	static bool SaveExistingAssetPackage(UObject* Asset, FString& OutError);
};
