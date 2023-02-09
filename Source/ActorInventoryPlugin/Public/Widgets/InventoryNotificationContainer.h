// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryNotificationContainer.generated.h"

class UInventoryNotification;

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryNotificationContainer : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category="Inventory|Notifications")
	void AddNewNotification(UInventoryNotification* Notification);

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Inventory|Notifications")
	void RemoveExpiredNotification(UInventoryNotification* Notification);

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory|Notifications")
	void DisplayNewNotification(FInventoryNotificationInfo Notification);

	FORCEINLINE TArray<UInventoryNotification*> GetNotifications() const
	{
		return Notifications;
	}
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UInventoryNotification*> Notifications;
	
};
