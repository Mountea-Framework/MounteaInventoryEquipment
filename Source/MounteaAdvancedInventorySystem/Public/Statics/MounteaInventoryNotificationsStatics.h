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

class IMounteaAdvancedInventoryUIInterface;
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void AddNotification(UObject* Container, UUserWidget* NewNotification);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveNotification(UObject* Container, UUserWidget* Notification);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void ClearNotifications(UObject* Container);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void SetNotificationsContainerVisibility(UObject* Container, const bool bShowContainer);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void CreateNotification(UObject* Notification, const FInventoryNotificationData& NotificationData, const TScriptInterface<IMounteaInventoryNotificationContainerWidgetInterface>& Container);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void ExpireNotification(UObject* Notification);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", meta=(CustomTag="MounteaK2Setter"))
	static void DeleteNotification(UObject* Notification);

	
};
