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

/** Parsed source records; reading never imports dependencies or writes assets. */
struct FMounteaInventoryReimportSource
{
	TMap<FGuid, FString> Items;
	TMap<FGuid, FString> Recipes;
	TMap<FGuid, TMap<FString, FString>> RecipeStations;
	FString Error;
};

struct FMounteaInventoryReimportResult
{
	TArray<UObject*> Updated;
	TArray<FString> Failures;
	int32 Skipped = 0;
	bool bCancelled = false;
};

struct FMounteaInventoryReimportHelpers
{
	static bool ReadSource(const FString& FilePath, FMounteaInventoryReimportSource& OutSource);
	/** Existing persistent assets only. Prompts once before overwriting dirty source-backed assets. */
	static FMounteaInventoryReimportResult Reimport(const TArray<UObject*>& Assets);
	static void ShowResult(const FMounteaInventoryReimportResult& Result);
};
