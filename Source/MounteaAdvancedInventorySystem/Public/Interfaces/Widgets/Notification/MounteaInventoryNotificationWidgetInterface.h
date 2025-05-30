// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "UObject/Interface.h"
#include "MounteaInventoryNotificationWidgetInterface.generated.h"

class IMounteaInventoryNotificationContainerWidgetInterface;
struct FInventoryNotificationData;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryNotificationWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventoryNotificationWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void CreateNotification(const FInventoryNotificationData& NotificationData, const TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface>& Container);
	virtual void CreateNotification_Implementation(const FInventoryNotificationData& NotificationData, const TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface>& Container) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void ExpireNotification();
	virtual void ExpireNotification_Implementation() = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void DeleteNotification();
	virtual void DeleteNotification_Implementation() = 0;
};
