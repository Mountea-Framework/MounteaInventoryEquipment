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
#include "Helpers/MounteaJsonDefinitionDependencyScanner.h"

class UMounteaAdvancedInventoryGlobalConfig;

/**
 * What blocks deleting a JSON object definition.
 */
struct FMounteaJsonDefinitionDeletionReport
{
	/** Other definitions whose IncludedDefinitions reference the key. */
	TArray<FString> IncludedBy;

	/** Blueprint and C++ usages from the last dependency scan. */
	TArray<FMounteaJsonDefinitionUsage> Usages;

	/** True once a dependency scan has completed at least once. */
	bool bScanned = false;

	/**
	 * @return True if any include or usage would break when the definition is deleted.
	 */
	bool HasBlockers() const
	{
		return IncludedBy.Num() > 0 || Usages.Num() > 0;
	}
};

/**
 * Collects what would break if a JSON object definition is deleted, and describes it for a confirmation dialog.
 */
class FMounteaJsonDefinitionDeletionCheck
{
public:

	/**
	 * Builds a deletion report for a definition key.
	 *
	 * @param Config Global config that holds the definitions.
	 * @param Key Definition key to check.
	 * @param Usages Blueprint and C++ usages for the key, usually from the dependency scanner.
	 * @return Report listing includes that reference the key and the supplied usages.
	 */
	static FMounteaJsonDefinitionDeletionReport Check(const UMounteaAdvancedInventoryGlobalConfig& Config, const FString& Key, const TArray<FMounteaJsonDefinitionUsage>& Usages);

	/**
	 * Builds the confirmation text for a report. Lists at most 10 entries.
	 *
	 * @param Key Definition key being deleted.
	 * @param Report Report returned by Check.
	 * @return Dialog text describing each blocker.
	 */
	static FText Describe(const FString& Key, const FMounteaJsonDefinitionDeletionReport& Report);
};
