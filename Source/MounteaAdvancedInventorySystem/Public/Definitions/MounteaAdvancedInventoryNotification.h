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
#include "GameplayTagContainer.h"
#include "Styling/SlateBrush.h"

#include "MounteaAdvancedInventoryNotification.generated.h"

class IMounteaAdvancedInventoryInterface;
class USoundBase;
class UUserWidget;

/**
* Types of inventory notifications.
*/
namespace MounteaInventoryNotificationBaseTypes
{
	const FString ItemNotFound(TEXT("Item Not Found"));
	const FString ItemAdded(TEXT("Item Added"));
	const FString ItemRemoved(TEXT("Item Removed"));
	const FString ItemPartiallyAdded(TEXT("Item Partially Added"));
	const FString ItemPartiallyRemoved(TEXT("Item Partially Removed"));
	const FString ItemNotUpdated(TEXT("Item Not Updated"));
	const FString QuantityLimitReached(TEXT("Quantity Limit Reached"));
	const FString InventoryLimitReached(TEXT("Inventory Limit Reached"));
	const FString DurabilityIncreased(TEXT("Item Durability Increased"));
	const FString DurabilityDecreased(TEXT("Item Durability Decreased"));
	const FString DurabilityMax(TEXT("Item Durability Max"));
	const FString DurabilityZero(TEXT("Item Durability Zero"));

	inline const TMap<FString, FString> NotificationMap = {
		{ TEXT("ItemNotFound"), ItemNotFound },
		{ TEXT("ItemAdded"), ItemAdded },
		{ TEXT("ItemRemoved"), ItemRemoved },
		{ TEXT("ItemPartiallyAdded"), ItemPartiallyAdded },
		{ TEXT("ItemPartiallyRemoved"), ItemPartiallyRemoved },
		{ TEXT("ItemNotUpdated"), ItemNotUpdated },
		{ TEXT("QuantityLimitReached"), QuantityLimitReached },
		{ TEXT("InventoryLimitReached"), InventoryLimitReached },
		{ TEXT("DurabilityIncreased"), DurabilityIncreased },
		{ TEXT("DurabilityDecreased"), DurabilityDecreased },
		{ TEXT("DurabilityMax"), DurabilityMax },
		{ TEXT("DurabilityZero"), DurabilityZero }
	};

	inline TArray<FString> GetAllNotificationTypes()
	{
		TArray<FString> Keys;
		NotificationMap.GenerateValueArray(Keys);
		return Keys;
	}
}


/**
 * EInventoryNotificationCategory defines the severity and visual presentation of inventory notifications.
 * Categories determine the styling, iconography, and user attention level for different types
 * of inventory system feedback (informational, warning, or error states).
 *
 * @see [Notification System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Notifications)
 * @see FInventoryNotificationData
 */
UENUM(BlueprintType)
enum class EInventoryNotificationCategory : uint8
{
	EINC_Info UMETA(DisplayName = "Info", Tooltip = "Information about successful operations"),
	EINC_Warning UMETA(DisplayName = "Warning", Tooltip = "Warning about partial success"),
	EINC_Error UMETA(DisplayName = "Error", Tooltip = "Error about failed operations"),
	
	Default UMETA(Hidden)
 };

/**
 * FInventoryNotificationStyle defines the visual and audio presentation of inventory notifications.
 * Styles control the appearance through icon brushes and audio feedback through sound assets
 * for consistent user experience across different notification types.
 *
 * @see [Notification System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Notifications)
 * @see FInventoryNotificationConfig
 */
USTRUCT(BlueprintType)
struct FInventoryNotificationStyle
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TSoftObjectPtr<USoundBase> NotificationSound;
};

/**
 * FInventoryNotificationConfig defines the behavior and presentation settings for inventory notifications.
 * Configurations control notification enablement, categorization, timing, appearance, and widget overrides
 * to provide flexible notification management across different inventory operations.
 *
 * @see [Notification System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Notifications)
 * @see FInventoryNotificationData
 */
USTRUCT(BlueprintType)
struct FInventoryNotificationConfig
{
	GENERATED_BODY()

	FInventoryNotificationConfig()
		: bIsEnabled(true)
		, NotificationCategory(EInventoryNotificationCategory::EINC_Info)
		, bShowProgressBar(false)
		, bCanBeClosed(true)
		, bHasDuration(false)
	{};

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
	TSoftClassPtr<UUserWidget> NotificationWidgetClassOverride;
};

/**
 * FInventoryNotificationData contains all information required to display and process inventory notifications.
 * Notification data includes type classification, display content, timing, item context, and payload
 * for comprehensive feedback about inventory system operations and state changes.
 *
 * @see [Notification System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Notifications)
 * @see IMounteaAdvancedInventoryInterface
 * @see EInventoryNotificationCategory
 */
USTRUCT(BlueprintType)
struct FInventoryNotificationData
{
	GENERATED_BODY()

	FInventoryNotificationData() = default;
	
	FInventoryNotificationData(
		const FString& InType,
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
	FString Type = TEXT("");
	 
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

public:
	FString ToString() const;
};
