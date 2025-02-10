// All rights reserved Dominik Morse 2024


#include "Statics/MounteaInventoryNotificationsStatics.h"

void UMounteaInventoryNotificationsStatics::CreateNotification(
	const TScriptInterface<IMounteaInventoryNotificationWidgetInterface>& Notification,
	const FInventoryNotificationData& NotificationData)
{
	if (Notification.GetObject())
		Notification->Execute_CreateNotification(Notification.GetObject(), NotificationData);
}
