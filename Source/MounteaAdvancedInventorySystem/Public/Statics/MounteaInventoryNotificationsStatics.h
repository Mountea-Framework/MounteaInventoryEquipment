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
#include "Interfaces/Widgets/Notification/MounteaInventoryNotificationWidgetInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryNotificationsStatics.generated.h"

class IMounteaAdvancedInventoryUIManagerInterface;
struct FInventoryNotificationData;
class UUserWidget;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryNotificationsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*************************************************************/
	/********************* TEMPLATES **********************/
	/*************************************************************/
public:
	template<typename ReturnType, typename Func, typename... Args>
	static ReturnType ExecuteIfImplements(UObject* Target, const TCHAR* FunctionName, Func Function, Args&&... args)
	{
		if (!IsValid(Target))
		{
			LOG_ERROR(TEXT("[%s] Invalid Target provided!"), FunctionName);
			if constexpr (!std::is_void_v<ReturnType>)
				return ReturnType{};
			else return;
		}

		if (Target->Implements<UMounteaInventoryNotificationWidgetInterface>())
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				Function(Target, Forward<Args>(args)...);
				return;
			}
			return Function(Target, Forward<Args>(args)...);
		}

		LOG_ERROR(TEXT("[%s] Target does not implement 'MounteaInventoryNotificationWidgetInterface'!"), FunctionName);
		if constexpr (!std::is_void_v<ReturnType>)
			return ReturnType{};
		else return;
	}

	/*************************************************************/
	/******************* BLUEPRINTABLE *******************/
	/*************************************************************/
	/**
	 * Adds a notification widget to a notification container.
	 *
	 * @param Container Notification container to update.
	 * @param NewNotification Notification widget to add.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification Container - Add Notification"))
	static void AddNotification(UObject* Container, UUserWidget* NewNotification);

	/**
	 * Removes a notification widget from a notification container.
	 *
	 * @param Container Notification container to update.
	 * @param Notification Notification widget to remove.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification Container - Remove Notification"))
	static void RemoveNotification(UObject* Container, UUserWidget* Notification);

	/**
	 * Clears all notifications from a notification container.
	 *
	 * @param Container Notification container to clear.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification Container - Clear Notifications"))
	static void ClearNotifications(UObject* Container);

	/**
	 * Updates notification container visibility.
	 *
	 * @param Container Notification container to update.
	 * @param bShowContainer Whether the container should be visible.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification Container - Set Visibility"))
	static void SetNotificationsContainerVisibility(UObject* Container, const bool bShowContainer);

	/**
	 * Initializes a notification widget from notification data.
	 *
	 * @param Notification Notification widget to initialize.
	 * @param NotificationData Data used to initialize the notification.
	 * @param Container Container that owns the notification.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification - Create Notification"))
	static void CreateNotification(UObject* Notification, const FInventoryNotificationData& NotificationData, const TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface>& Container);
	
	/**
	 * Expires a notification widget.
	 *
	 * @param Notification Notification widget to expire.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification - Expire Notification"))
	static void ExpireNotification(UObject* Notification);

	/**
	 * Deletes a notification widget.
	 *
	 * @param Notification Notification widget to delete.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification",
		meta=(MounteaSetter),
		meta=(DisplayName="Notification - Delete Notification"))
	static void DeleteNotification(UObject* Notification);

	
};
