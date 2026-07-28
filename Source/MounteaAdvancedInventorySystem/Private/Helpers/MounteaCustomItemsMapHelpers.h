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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"

/**
 * Shared, static logic for the Custom Items Map (GameplayTag -> set of Guids) exposed by
 * IMounteaAdvancedCustomItemsMapInterface. Each implementing manager component (e.g. UMounteaInventoryUIComponent,
 * UMounteaCraftingParticipantUIComponent) owns its own TMap<FGameplayTag, FInventoryUICustomData> instance and
 * passes it by reference into these functions - storage is duplicated per manager, logic is shared.
 * All methods are static; this struct is never instantiated.
 */
struct FMounteaCustomItemsMapHelpers
{
	using FCustomItemsMap = TMap<FGameplayTag, FInventoryUICustomData>;

	static void AddCustomItem(FCustomItemsMap& Map, const FGameplayTag& ItemTag, const FGuid& ItemId);
	static void AppendCustomItems(FCustomItemsMap& Map, const FCustomItemsMap& OtherItems);
	static void ClearCustomItems(FCustomItemsMap& Map);
	static bool RemoveCustomItem(FCustomItemsMap& Map, const FGameplayTag& ItemTag, const FGuid& ItemId);
	static bool IsItemStored(const FCustomItemsMap& Map, const FGameplayTag& ItemTag, const FGuid& ItemId);
};
