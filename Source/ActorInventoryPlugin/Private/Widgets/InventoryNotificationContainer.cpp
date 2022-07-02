// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryNotificationContainer.h"

#include "Widgets/InventoryNotification.h"

void UInventoryNotificationContainer::AddNewNotification(UInventoryNotification* Notification)
{
	if (Notifications.Contains(Notification))
	{
		return;
	}

	Notifications.Emplace(Notification);

	Notification->OnNotificationExpired.AddUniqueDynamic(this, &UInventoryNotificationContainer::RemoveExpiredNotification);
}

void UInventoryNotificationContainer::RemoveExpiredNotification_Implementation(UInventoryNotification* Notification)
{
	if (Notifications.Contains(Notification))
	{
		Notifications.RemoveSingle(Notification);
	}
}

void UInventoryNotificationContainer::DisplayNewNotification_Implementation(FInventoryNotificationInfo Notification)
{
	//...
}
