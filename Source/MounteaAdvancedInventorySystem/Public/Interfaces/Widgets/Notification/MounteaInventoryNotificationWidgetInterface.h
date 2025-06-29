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
