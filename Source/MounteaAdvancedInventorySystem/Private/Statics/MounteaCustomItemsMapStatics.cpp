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

#include "Statics/MounteaCustomItemsMapStatics.h"

TMap<FGameplayTag, FInventoryUICustomData> UMounteaCustomItemsMapStatics::GetCustomItemsMap(
	const TScriptInterface<IMounteaAdvancedCustomItemsMapInterface>& Target)
{
	return Target.GetObject() ?
		IMounteaAdvancedCustomItemsMapInterface::Execute_GetCustomItemsMap(Target.GetObject()) : TMap<FGameplayTag, FInventoryUICustomData>();
}

void UMounteaCustomItemsMapStatics::AddCustomItem(const TScriptInterface<IMounteaAdvancedCustomItemsMapInterface>& Target,
	const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (Target.GetObject())
		IMounteaAdvancedCustomItemsMapInterface::Execute_AddCustomItemToMap(Target.GetObject(), ItemTag, ItemId);
}

void UMounteaCustomItemsMapStatics::AppendCustomItems(const TScriptInterface<IMounteaAdvancedCustomItemsMapInterface>& Target,
	const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems)
{
	if (Target.GetObject())
		IMounteaAdvancedCustomItemsMapInterface::Execute_AppendCustomItemsMap(Target.GetObject(), OtherItems);
}

void UMounteaCustomItemsMapStatics::ClearCustomItems(const TScriptInterface<IMounteaAdvancedCustomItemsMapInterface>& Target)
{
	if (Target.GetObject())
		IMounteaAdvancedCustomItemsMapInterface::Execute_ClearCustomItemsMap(Target.GetObject());
}

bool UMounteaCustomItemsMapStatics::RemoveCustomItem(const TScriptInterface<IMounteaAdvancedCustomItemsMapInterface>& Target,
	const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (Target.GetObject())
		return IMounteaAdvancedCustomItemsMapInterface::Execute_RemoveCustomItemFromMap(Target.GetObject(), ItemTag, ItemId);
	return false;
}

bool UMounteaCustomItemsMapStatics::IsItemStoredInCustomMap(const TScriptInterface<IMounteaAdvancedCustomItemsMapInterface>& Target,
	const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (Target.GetObject())
		return IMounteaAdvancedCustomItemsMapInterface::Execute_IsItemStoredInCustomMap(Target.GetObject(), ItemTag, ItemId);
	return false;
}
