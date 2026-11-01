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

#include "Helpers/MounteaJsonDefinitionDeletionCheck.h"

#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"

FMounteaJsonDefinitionDeletionReport FMounteaJsonDefinitionDeletionCheck::Check(const UMounteaAdvancedInventoryGlobalConfig& Config, const FString& Key, const TArray<FMounteaJsonDefinitionUsage>& Usages)
{
	FMounteaJsonDefinitionDeletionReport report;
	report.Usages = Usages;
	report.bScanned = !FMounteaJsonDefinitionDependencyScanner::GetLastScanTime().IsEmpty();

	for (const TPair<FString, FMounteaJsonObjectDefinition>& pair : Config.JsonObjectDefinitions)
	{
		if (pair.Key == Key)
			continue;

		for (const FMounteaJsonObjectDefinitionInclude& include : pair.Value.IncludedDefinitions)
		{
			if (include.DefinitionKey == Key)
			{
				report.IncludedBy.AddUnique(pair.Key);
				break;
			}
		}
	}

	return report;
}

FText FMounteaJsonDefinitionDeletionCheck::Describe(const FString& Key, const FMounteaJsonDefinitionDeletionReport& Report)
{
	constexpr int32 MaxListedEntries = 10;

	TArray<FString> lines;
	for (const FString& includer : Report.IncludedBy)
	{
		lines.Add(FString::Printf(TEXT("Included by: %s"), *includer));
	}

	for (const FMounteaJsonDefinitionUsage& usage : Report.Usages)
	{
		lines.Add(FString::Printf(TEXT("%s: %s (x%d)"), *usage.Source, *usage.Path, usage.Count));
	}

	FString text = FString::Printf(TEXT("'%s' is still in use:\n"), *Key);
	const int32 shown = FMath::Min(lines.Num(), MaxListedEntries);
	for (int32 index = 0; index < shown; index++)
	{
		text += lines[index] + TEXT("\n");
	}

	if (lines.Num() > shown)
		text += FString::Printf(TEXT("...and %d more\n"), lines.Num() - shown);

	if (!Report.bScanned)
		text += TEXT("\nNo dependency scan has run yet, so Blueprint and C++ usages are unknown.\n");

	text += TEXT("\nDelete anyway?");
	return FText::FromString(text);
}
