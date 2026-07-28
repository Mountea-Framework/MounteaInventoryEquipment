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

#include "Helpers/MounteaCustomItemsMapHelpers.h"

#include "Algo/AnyOf.h"

void FMounteaCustomItemsMapHelpers::AddCustomItem(FCustomItemsMap& Map, const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (FInventoryUICustomData* customData = Map.Find(ItemTag))
	{
		if (!customData->StoredIds.Contains(ItemId))
			customData->StoredIds.Add(ItemId);
	}
	else
	{
		FInventoryUICustomData newData;
		newData.StoredIds.Add(ItemId);

		Map.Add(ItemTag, MoveTemp(newData));
	}
}

void FMounteaCustomItemsMapHelpers::AppendCustomItems(FCustomItemsMap& Map, const FCustomItemsMap& OtherItems)
{
	Map.Append(OtherItems);
}

void FMounteaCustomItemsMapHelpers::ClearCustomItems(FCustomItemsMap& Map)
{
	Map.Reset();
}

bool FMounteaCustomItemsMapHelpers::RemoveCustomItem(FCustomItemsMap& Map, const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	FInventoryUICustomData* foundData = Map.Find(ItemTag);
	if (!foundData || foundData->StoredIds.IsEmpty())
		return false;

	const int32 removedCount = foundData->StoredIds.RemoveAll([&ItemId](const FGuid& storedId)
	{
		return storedId == ItemId;
	});

	if (removedCount > 0 && foundData->StoredIds.Num() == 0)
		Map.Remove(ItemTag);

	return removedCount > 0;
}

bool FMounteaCustomItemsMapHelpers::IsItemStored(const FCustomItemsMap& Map, const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	const FInventoryUICustomData* foundData = Map.Find(ItemTag);
	if (!foundData)
		return false;

	return Algo::AnyOf(foundData->StoredIds, [&ItemId](const FGuid& storedId)
	{
		return storedId == ItemId;
	});
}
