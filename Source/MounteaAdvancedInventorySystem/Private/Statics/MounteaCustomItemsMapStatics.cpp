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

#include "Logs/MounteaAdvancedInventoryLog.h"

bool UMounteaCustomItemsMapStatics::ValidateTarget(const UObject* Target, const TCHAR* CallingFunction)
{
	if (!IsValid(Target))
	{
		LOG_WARNING(TEXT("[%s] Target is invalid."), CallingFunction)
		return false;
	}

	if (!Target->Implements<UMounteaAdvancedCustomItemsMapInterface>())
	{
		LOG_WARNING(TEXT("[%s] Target '%s' does not implement IMounteaAdvancedCustomItemsMapInterface."), CallingFunction, *Target->GetName())
		return false;
	}

	return true;
}

TMap<FGameplayTag, FInventoryUICustomData> UMounteaCustomItemsMapStatics::GetCustomItemsMap(UObject* Target)
{
	if (!ValidateTarget(Target, TEXT("Custom Items Map - Get Custom Items")))
		return TMap<FGameplayTag, FInventoryUICustomData>();

	return IMounteaAdvancedCustomItemsMapInterface::Execute_GetCustomItemsMap(Target);
}

void UMounteaCustomItemsMapStatics::AddCustomItem(UObject* Target, const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (!ValidateTarget(Target, TEXT("Custom Items Map - Add Custom Item")))
		return;

	IMounteaAdvancedCustomItemsMapInterface::Execute_AddCustomItemToMap(Target, ItemTag, ItemId);
}

void UMounteaCustomItemsMapStatics::AppendCustomItems(UObject* Target, const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems)
{
	if (!ValidateTarget(Target, TEXT("Custom Items Map - Append Custom Items")))
		return;

	IMounteaAdvancedCustomItemsMapInterface::Execute_AppendCustomItemsMap(Target, OtherItems);
}

void UMounteaCustomItemsMapStatics::ClearCustomItems(UObject* Target)
{
	if (!ValidateTarget(Target, TEXT("Custom Items Map - Clear Custom Items")))
		return;

	IMounteaAdvancedCustomItemsMapInterface::Execute_ClearCustomItemsMap(Target);
}

bool UMounteaCustomItemsMapStatics::RemoveCustomItem(UObject* Target, const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (!ValidateTarget(Target, TEXT("Custom Items Map - Remove Custom Item")))
		return false;

	return IMounteaAdvancedCustomItemsMapInterface::Execute_RemoveCustomItemFromMap(Target, ItemTag, ItemId);
}

bool UMounteaCustomItemsMapStatics::IsItemStoredInCustomMap(UObject* Target, const FGameplayTag& ItemTag, const FGuid& ItemId)
{
	if (!ValidateTarget(Target, TEXT("Custom Items Map - Is Item Stored In Custom Map")))
		return false;

	return IMounteaAdvancedCustomItemsMapInterface::Execute_IsItemStoredInCustomMap(Target, ItemTag, ItemId);
}
