// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventorySettingsConfig.generated.h"

/**
* Data Asset that manages configuration settings for Mountea Advanced Inventory System.
* Defines inventory types, rarities, and categories with their respective settings.
*
* @see UDataAsset
* @see FInventoryTypeConfig
* @see FInventoryRarity
* @see FInventoryCategory
* @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Settings-Config
*/
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining inventory types, rarities and categories."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsConfig();

public:

	// --- Types ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Types", meta=(ForceInlineRow, NoResetToDefault))
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	// --- Rarities ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rarities", meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryRarity> AllowedRarities;

	// --- Categories ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Categories", meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryCategory> AllowedCategories;

	// --- Limits ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limits", meta=(NoResetToDefault))
	float InventoryBaseWeightLimit = 150.f;

	// --- User Interface ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserInterface|Visual")
	FIntPoint InventoryGridDimensions = FIntPoint(5, 8);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserInterface|Visual")
	FVector2D ItemSlotSize = FVector2D(128.0f, 128.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserInterface|Visual", meta=(UIMin=0.1f,UIMax=10.f,ClampMin=0.1f,ClampMax=10.f))
	float ItemSlotAspectRatio = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Wrapper", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryBaseWidgetInterface"))
	TSoftClassPtr<UUserWidget> UserInterfaceWrapperClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Config", meta=(NoResetToDefault))
	TSet<FString> WidgetCommands;

	// --- User Interface: Items

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory|Items", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemSlotsWrapperWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory|Items", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemSlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory|Items", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory|Items", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemsGridWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemsGridWidgetClass;

	// --- User Interface: Categories

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory|Categories", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoriesWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryCategoriesWrapperWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Inventory|Categories", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryCategoryWidgetClass;
	
	// --- Notifications ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Notifications", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetContainerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Notifications", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notifications|Style", meta=(ForceInlineRow))
	TMap<EInventoryNotificationCategory, FInventoryNotificationStyle> NotificationCategoryStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notifications|Config", meta=(NoResetToDefault))
	TSet<FString> NotificationTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notifications|Config", meta=(ForceInlineRow), meta=(GetKeyOptions="GetNotificationTypes"))
	TMap<FString, FInventoryNotificationConfig> NotificationConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications|UserInterface")
	FVector2D NotificationSizeOverride = FVector2D(400.0f, 50.0f);
	
protected:

	void ValidateInventoryTypes();
	static FInventoryTypeConfig GetDefaultConfigForType(const EInventoryType Type);

	static void SetupPlayerConfig(FInventoryTypeConfig& Config);
	static void SetupNPCConfig(FInventoryTypeConfig& Config);
	static void SetupStorageConfig(FInventoryTypeConfig& Config);
	static void SetupMerchantConfig(FInventoryTypeConfig& Config);
	static void SetupLootConfig(FInventoryTypeConfig& Config);
	static void SetupSpecializedConfig(FInventoryTypeConfig& Config);

	void SetupWidgetCommands();

	UFUNCTION()
	TArray<FString> GetNotificationTypes() const;

#if WITH_EDITOR
public:
	/** Sets default values. Default values can be edited. If requested then Default values will override all `Categories` and `Rarities`! */
	UFUNCTION(CallInEditor, Category="Default Values")
	void SetDefaultValues();

	UFUNCTION(CallInEditor, Category="Default Values")
	void SetDefaultNotificationConfig();
protected:
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
