// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Widgets/MounteaInventoryNotificationWidgetInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryNotificationsStatics.generated.h"

struct FInventoryNotificationData;

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

		if (Target->Implements<IMounteaInventoryNotificationWidgetInterface>())
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
	static void CreateNotification(const TScriptInterface<IMounteaInventoryNotificationWidgetInterface>& Notification, const FInventoryNotificationData& NotificationData);
};
