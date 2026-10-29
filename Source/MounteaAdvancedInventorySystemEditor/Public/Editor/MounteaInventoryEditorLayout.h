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
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

/** Common presentation and actions for the item and crafting recipe editors. */
struct FMounteaInventoryEditorToolbarActions
{
	FOnClicked Create, Save, SaveAll, Import, Export, Close, Help;
	TAttribute<bool> CanSave = false;
	TAttribute<bool> CanSaveAll = false;
};

namespace MounteaInventoryEditorLayout
{
	TSharedRef<SWidget> MakeToolbar(const FMounteaInventoryEditorToolbarActions& Actions, TSharedRef<SWidget> Search);
	TSharedRef<SWidget> MakeBrowserDetails(TSharedRef<SWidget> Browser, TSharedRef<SWidget> Details,
		TAttribute<float> BrowserMinimum, TAttribute<float> DetailsMinimum);
	TSharedRef<SWidget> MakeLayout(TSharedRef<SWidget> Toolbar, TSharedRef<SWidget> Content);
}
