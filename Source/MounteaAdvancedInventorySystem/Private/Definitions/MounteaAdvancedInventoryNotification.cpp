// All rights reserved Dominik Morse 2024


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


