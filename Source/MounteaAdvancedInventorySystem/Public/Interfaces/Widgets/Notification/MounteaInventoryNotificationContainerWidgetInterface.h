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
#include "UObject/Interface.h"
#include "MounteaInventoryNotificationContainerWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;
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

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Notification")
	void SetNotificationsContainerVisibility(const bool bShowContainer);
	virtual void SetNotificationsContainerVisibility_Implementation(const bool bShowContainer) = 0;
};
