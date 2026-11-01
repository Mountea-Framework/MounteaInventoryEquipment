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
#include "Helpers/MounteaJsonDefinitionDependencyScanner.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Blueprint usages that share one content folder.
 */
struct FMounteaJsonDependencyGroup
{
	/** Content folder shared by every usage in this group, e.g. /Game/Items. */
	FString FolderPath;

	/** Blueprint usages in this folder, sorted by object path. */
	TArray<FMounteaJsonDefinitionUsage> Usages;
};

/**
 * Content of the modal that lists where a JSON object definition key is used.
 * Blueprints are grouped by folder and open on click; C++ usages are listed as plain, non-clickable rows.
 */
class SMounteaJsonDefinitionDependencyList : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SMounteaJsonDefinitionDependencyList) {}
		SLATE_ARGUMENT(FString, DefinitionKey)
		SLATE_ARGUMENT(FText, Description)
		SLATE_ARGUMENT(TWeakPtr<SWindow>, OwnerWindow)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/**
	 * Opens the dependency list as a modal window for a definition key.
	 *
	 * @param DefinitionKey Definition key whose usages are shown.
	 * @param Description Definition description shown above the usages; may be empty.
	 */
	static void ShowModal(const FString& DefinitionKey, const FText& Description);

	/**
	 * Groups Blueprint usages by content folder. C++ usages are ignored.
	 *
	 * @param Usages Usages returned by the dependency scanner.
	 * @return Groups sorted by folder, each with usages sorted by object path.
	 */
	static TArray<FMounteaJsonDependencyGroup> GroupBlueprintUsages(const TArray<FMounteaJsonDefinitionUsage>& Usages);
};
