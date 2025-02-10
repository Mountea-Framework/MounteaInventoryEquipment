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
	EINT_ItemNotFound UMETA(DisplayName = "Item Not Found", Tooltip = "Item was not found in Inventory"),
	EINT_ItemAdded UMETA(DisplayName = "Item Added", Tooltip = "Item was successfully added to inventory"),
	EINT_ItemRemoved UMETA(DisplayName = "Item Removed", Tooltip = "Item was successfully removed from inventory"), 
	EINT_ItemPartiallyAdded UMETA(DisplayName = "Item Partially Added", Tooltip = "Only part of stack could be added"),
	EINT_ItemPartiallyRemoved UMETA(DisplayName = "Item Partially Removed", Tooltip = "Only part of stack could be removed"),
	EINT_ItemNotUpdated UMETA(DisplayName = "Item Not Updated", Tooltip = "Operation failed completely"),
	EINT_QuantityLimitReached UMETA(DisplayName = "Quantity Limit Reached", Tooltip = "Maximum quantity reached for item type"),
	EINT_InventoryLimitReached UMETA(DisplayName = "Inventory Limit Reached", Tooltip = "Inventory cannot hold more items"),

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
struct FInventoryNotificationStyle
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TSoftObjectPtr<USoundBase> NotificationSound;
};

USTRUCT(BlueprintType)
struct FInventoryNotificationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	uint8 bIsEnabled : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EInventoryNotificationCategory NotificationCategory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText MessageTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText MessageTemplate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta=(EditCondition="bHasDuration"))
	uint8 bShowProgressBar : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	uint8 bCanBeClosed : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	uint8 bHasDuration : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta=(UIMin=0.1,ClampMin=0.1), meta=(EditCondition="bHasDuration"))
	float DefaultDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FLinearColor BackgroundColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.9f);
	
	UPROPERTY(EditAnywhere, Category="Settings", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetClassOverride;
};

USTRUCT(BlueprintType)
struct FInventoryNotificationData
{
	GENERATED_BODY()

	FInventoryNotificationData() = default;
	
	FInventoryNotificationData(
		const EInventoryNotificationType InType,
		const EInventoryNotificationCategory InCategory,
		const FText& InTitle,
		const FText& InText,
		const FGuid& InItemGuid,
		const TScriptInterface<IMounteaAdvancedInventoryInterface> InSourceInventory,
		const int32 InDeltaAmount = 0,
		const float InDuration = 3.0f,
		const FInventoryNotificationConfig& SourceConfig = FInventoryNotificationConfig(),
		UObject* InPayload = nullptr
		) : Type(InType)
			, Category(InCategory)
			, NotificationTitle(InTitle)
			, NotificationText(InText)
			, Duration(InDuration)
			, ItemGuid(InItemGuid)
			, SourceInventory(InSourceInventory)
			, DeltaAmount(InDeltaAmount)
			, NotificationPayload(InPayload)
			, NotificationConfig(SourceConfig)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	EInventoryNotificationType Type = EInventoryNotificationType::Default;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	EInventoryNotificationCategory Category = EInventoryNotificationCategory::EINC_Info;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	FText NotificationTitle;
	 
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
	int32 DeltaAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
	TObjectPtr<UObject> NotificationPayload;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Notification")
	FInventoryNotificationConfig NotificationConfig;
};
