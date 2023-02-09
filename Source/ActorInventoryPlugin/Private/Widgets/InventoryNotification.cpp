// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Widgets/InventoryNotification.h"
#include "TimerManager.h"

void UInventoryNotification::NativeConstruct()
{
	Super::NativeConstruct();

	if (!FMath::IsNearlyZero(NotificationInfo.ShowDuration))
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer
			(
				Expiration_TimeHandle, this, &UInventoryNotification::OnNotificationExpiredCallback, NotificationInfo.ShowDuration, false
			);
		}
	}
}

void UInventoryNotification::OnNotificationExpiredCallback()
{
	OnNotificationExpired.Broadcast(this);
}
