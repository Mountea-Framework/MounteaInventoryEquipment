// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryNotificationsStatics.h"

#include "Interfaces/Widgets/MounteaInventoryNotificationContainerWidgetInterface.h"

void UMounteaInventoryNotificationsStatics::CreateNotification(
	UObject* Notification,	const FInventoryNotificationData& NotificationData, const TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface>& Container)
{
	if (!IsValid(Notification))
		return;

	if (Notification->Implements<UMounteaInventoryNotificationWidgetInterface>())
		return IMounteaInventoryNotificationWidgetInterface::Execute_CreateNotification(Notification, NotificationData, Container);
}

void UMounteaInventoryNotificationsStatics::ExpireNotification(UObject* Notification)
{
	if (!IsValid(Notification))
		return;

	if (Notification->Implements<UMounteaInventoryNotificationWidgetInterface>())
		return IMounteaInventoryNotificationWidgetInterface::Execute_ExpireNotification(Notification);
}

void UMounteaInventoryNotificationsStatics::DeleteNotification(UObject* Notification)
{
	if (!IsValid(Notification))
		return;

	if (Notification->Implements<UMounteaInventoryNotificationWidgetInterface>())
		return IMounteaInventoryNotificationWidgetInterface::Execute_DeleteNotification(Notification);
}

void UMounteaInventoryNotificationsStatics::AddNotification(UObject* Container, UUserWidget* NewNotification)
{
	if (!IsValid(Container))
		return;

	if (Container->Implements<UMounteaInventoryNotificationContainerWidgetInterface>())
		return IMounteaInventoryNotificationContainerWidgetInterface::Execute_AddNotification(Container, NewNotification);
}

void UMounteaInventoryNotificationsStatics::RemoveNotification(UObject* Container, UUserWidget* Notification)
{
	if (!IsValid(Container))
		return;

	if (Container->Implements<UMounteaInventoryNotificationContainerWidgetInterface>())
		return IMounteaInventoryNotificationContainerWidgetInterface::Execute_RemoveNotification(Container, Notification);
}

void UMounteaInventoryNotificationsStatics::ClearNotifications(UObject* Container)
{
	if (!IsValid(Container))
		return;

	if (Container->Implements<UMounteaInventoryNotificationContainerWidgetInterface>())
		return IMounteaInventoryNotificationContainerWidgetInterface::Execute_ClearNotifications(Container);
}

void UMounteaInventoryNotificationsStatics::SetNotificationsContainerVisibility(UObject* Container,
	const bool bShowContainer)
{
	if (!IsValid(Container))
		return;

	if (Container->Implements<UMounteaInventoryNotificationContainerWidgetInterface>())
		return IMounteaInventoryNotificationContainerWidgetInterface::Execute_SetNotificationsContainerVisibility(Container, bShowContainer);
}
