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


#include "Definitions/MounteaAdvancedInventoryNotification.h"

FString FInventoryNotificationData::ToString() const
{
	const UEnum* EnumPtr = StaticEnum<EInventoryNotificationCategory>();
	FString categoryString = EnumPtr ? EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Category)).ToString() : TEXT("Unknown");

	return FString::Printf(
		TEXT("Type: %s\nCategory: %s\nTitle: %s\nText: %s\nDuration: %.2f\nItemGuid: %s\nDeltaAmount: %d\nPayload: %s\nTags: %s"),
		*Type,
		*categoryString,
		*NotificationTitle.ToString(),
		*NotificationText.ToString(),
		Duration,
		*ItemGuid.ToString(),
		DeltaAmount,
		NotificationPayload ? *NotificationPayload->GetName() : TEXT("None"),
		*NotificationTags.ToString()
	);
}


