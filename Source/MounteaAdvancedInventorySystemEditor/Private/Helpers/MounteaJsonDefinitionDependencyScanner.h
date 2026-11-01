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

#pragma once

#include "CoreMinimal.h"

/**
 * One place a JSON object definition key is used.
 */
struct FMounteaJsonDefinitionUsage
{
	/** "Blueprint" or "C++". */
	FString Source;

	/** Blueprint object path or C++ source file path. */
	FString Path;

	/** Number of uses found at this location. */
	int32 Count = 0;
};

/**
 * Finds where JSON object definition keys are used and caches the result in the editor's Saved folder.
 * Blueprints are scanned in time-sliced ticks on the game thread; C++ sources are scanned on the thread pool.
 */
class FMounteaJsonDefinitionDependencyScanner
{
public:

	/** Connects the scanner to the global config's Recalculate Dependencies request. */
	static void Bind();

	/** Disconnects the scanner from the global config. */
	static void Unbind();

	/**
	 * Starts a scan for the supplied definition keys. Ignored while another scan is running.
	 *
	 * @param DefinitionKeys Definition keys to search for.
	 */
	static void StartScan(const TArray<FString>& DefinitionKeys);

	/**
	 * Returns the cached usages recorded for a definition key.
	 *
	 * @param DefinitionKey Definition key to look up.
	 * @return Usages found by the last completed scan; empty if none were recorded.
	 */
	static TArray<FMounteaJsonDefinitionUsage> GetUsages(const FString& DefinitionKey);

	/**
	 * Returns when the last completed scan finished.
	 *
	 * @return Local timestamp of the last completed scan, or an empty string if no scan has completed.
	 */
	static FString GetLastScanTime();

	/**
	 * Counts quoted occurrences of a definition key inside source text.
	 *
	 * @param Content Source text to search.
	 * @param Key Definition key to count.
	 * @return Number of quoted occurrences of the key.
	 */
	static int32 CountKeyLiterals(const FString& Content, const FString& Key);
};
