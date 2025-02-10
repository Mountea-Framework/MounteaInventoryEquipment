// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "UObject/Interface.h"
#include "MounteaInventoryNotificationWidgetInterface.generated.h"

struct FInventoryNotificationData;
// This class does not need to be modified.
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void CreateNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;
};
