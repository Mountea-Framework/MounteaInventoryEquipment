// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryNotification.h"


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
