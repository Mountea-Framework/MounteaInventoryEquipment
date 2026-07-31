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
#include "MounteaAdvancedInventoryWorkspaceEditorStatics.generated.h"

class UMounteaInventoryItemTemplate;
class UMounteaRecipeTemplate;
class UMounteaAdvancedInventoryLoadoutConfig;

/**
 * Editor-only static helpers for importing a full-workspace .mnteainventory file.
 *
 * Unlike the single-type importers, a workspace bundles items/, crafting/ (recipes), and
 * loadouts/ as a flat GUID-keyed zip (not a zip-of-zips) alongside a combined taxonomy.json
 * and an informational manifest.json. All entity-level parsing is reused as-is from the
 * existing per-type EditorStatics classes; this class only orchestrates: import every item
 * first, then every recipe and loadout against the resulting GUID->template map.
 *
 * Import is best-effort per entity, not atomic across the whole workspace: a malformed
 * recipe or loadout is skipped (logged) rather than aborting the rest of the import.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventoryWorkspaceEditorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Shows an open-file dialog (filtered to *.mnteainventory) and a target-folder picker, then imports. */
	static bool ImportWorkspaceFromFile(
		TArray<UMounteaInventoryItemTemplate*>& OutItems,
		TArray<UMounteaRecipeTemplate*>& OutRecipes,
		TArray<UMounteaAdvancedInventoryLoadoutConfig*>& OutLoadouts,
		FString& OutErrorMessage);

	/**
	 * Main entry point. Imports every item, recipe, and loadout found in the workspace file,
	 * auto-sorted under TargetRootFolder/Items, TargetRootFolder/Recipes, TargetRootFolder/Loadouts.
	 */
	static bool ImportWorkspaceFromFilePath(
		const FString& FilePath,
		const FString& TargetRootFolder,
		TArray<UMounteaInventoryItemTemplate*>& OutItems,
		TArray<UMounteaRecipeTemplate*>& OutRecipes,
		TArray<UMounteaAdvancedInventoryLoadoutConfig*>& OutLoadouts,
		FString& OutErrorMessage);
};
