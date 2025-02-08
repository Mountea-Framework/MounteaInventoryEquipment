// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaAdvancedInventoryNotification.generated.h"

class IMounteaAdvancedInventoryInterface;
/**
* Types of inventory notifications.
*/
UENUM(BlueprintType)
enum class EInventoryNotificationType : uint8
{
	EINT_ItemAdded UMETA(DisplayName = "Item Added", Tooltip = "Item was successfully added to inventory"),
	EINT_ItemRemoved UMETA(DisplayName = "Item Removed", Tooltip = "Item was successfully removed from inventory"), 
	EINT_ItemPartiallyAdded UMETA(DisplayName = "Item Partially Added", Tooltip = "Only part of stack could be added"),
	EINT_ItemPartiallyRemoved UMETA(DisplayName = "Item Partially Removed", Tooltip = "Only part of stack could be removed"),
	EINT_ItemNotUpdated UMETA(DisplayName = "Item Not Updated", Tooltip = "Operation failed completely"),

	Default UMETA(Hidden)
 };

/**
* Categories of inventory notifications.
*/
UENUM(BlueprintType)
enum class EInventoryNotificationCategory : uint8
{
	EINC_Info UMETA(DisplayName = "Info", Tooltip = "Information about successful operations"),
	EINC_Warning UMETA(DisplayName = "Warning", Tooltip = "Warning about partial success"),
	EINC_Error UMETA(DisplayName = "Error", Tooltip = "Error about failed operations"),
	
	Default UMETA(Hidden)
 };

USTRUCT(BlueprintType)
struct FInventoryNotificationData
{
	GENERATED_BODY()

	FInventoryNotificationData() = default;
	
	FInventoryNotificationData(
		const EInventoryNotificationType InType,
		const EInventoryNotificationCategory InCategory,
		const FText& InText,
		const FGuid& InItemGuid,
		const TScriptInterface<IMounteaAdvancedInventoryInterface> InSourceInventory,
		const int32 InRequestedAmount = 0,
		const int32 InProcessedAmount = 0,
		const float InDuration = 3.0f
		) : Type(InType)
		 , Category(InCategory)
		 , NotificationText(InText)
		 , Duration(InDuration)
		 , ItemGuid(InItemGuid)
		 , SourceInventory(InSourceInventory)
		 , RequestedAmount(InRequestedAmount)
		 , ProcessedAmount(InProcessedAmount)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	EInventoryNotificationType Type = EInventoryNotificationType::Default;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	EInventoryNotificationCategory Category = EInventoryNotificationCategory::EINC_Info;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	FText NotificationText;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	float Duration = 3.0f;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	FGameplayTagContainer NotificationTags;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	FGuid ItemGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	TScriptInterface<IMounteaAdvancedInventoryInterface> SourceInventory;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	int32 RequestedAmount = 0;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	int32 ProcessedAmount = 0;
};
