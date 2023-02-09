// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryNotification.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationExpired, class UInventoryNotification*, EppiredNotification);

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryNotification : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	FInventoryNotificationInfo NotificationInfo;
		
	UPROPERTY()
	FTimerHandle Expiration_TimeHandle;

private:

	void OnNotificationExpiredCallback();

public:

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnNotificationExpired OnNotificationExpired;
};
