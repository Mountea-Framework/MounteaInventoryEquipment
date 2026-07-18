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
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoryWidgetInterface.h"

/**
 * Shared, static logic for the per-Category-widget OnCategorySelected event registry.
 * Each manager component (e.g. UMounteaInventoryUIComponent, UMounteaCraftingParticipantUIComponent)
 * owns its own TMap<TWeakObjectPtr<UObject>, FOnCategoryWidgetSelected> instance and passes it by
 * reference into these functions - storage is duplicated per manager, logic is shared.
 * All methods are static; this struct is never instantiated.
 */
struct FMounteaCategoryWidgetEventHelpers
{
	using FCategoryWidgetEventRegistry = TMap<TWeakObjectPtr<UObject>, FOnCategoryWidgetSelected>;

	static bool BindCategoryWidgetSelected(FCategoryWidgetEventRegistry& Registry, UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding);
	static bool UnbindCategoryWidgetSelected(FCategoryWidgetEventRegistry& Registry, UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding);
	static bool TriggerCategoryWidgetSelected(FCategoryWidgetEventRegistry& Registry, UObject* CategoryWidget, const FString& CategoryId);

	/** Removes entries whose TWeakObjectPtr key has gone stale (widget garbage-collected). Called on every access above. */
	static int32 PruneStaleEntries(FCategoryWidgetEventRegistry& Registry);
};
