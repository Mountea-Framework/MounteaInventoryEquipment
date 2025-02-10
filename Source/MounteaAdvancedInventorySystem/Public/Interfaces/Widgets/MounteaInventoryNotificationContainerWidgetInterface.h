// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryNotificationContainerWidgetInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryNotificationContainerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventoryNotificationContainerWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void AddNotification(UUserWidget* NewNotification);
	virtual void AddNotification_Implementation(UUserWidget* NewNotification) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void RemoveNotification(UUserWidget* Notification);
	virtual void RemoveNotification_Implementation(UUserWidget* Notification) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void ClearNotifications();
	virtual void ClearNotifications_Implementation() = 0;
};
