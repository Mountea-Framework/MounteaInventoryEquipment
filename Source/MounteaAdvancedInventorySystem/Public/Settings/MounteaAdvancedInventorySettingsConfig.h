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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Types", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	// --- Rarities ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rarities", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryRarity> AllowedRarities;

	// --- Categories ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Categories", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryCategory> AllowedCategories;

	// --- Limits ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limits", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	float InventoryBaseWeightLimit = 150.f;

	// --- User Interface ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryBaseWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryWidgetClass;

	// --- Notifications ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Notifications", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetContainerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserInterface|Classes|Notifications", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notifications|Style", meta=(ShowOnlyInnerProperties, ForceInlineRow))
	TMap<EInventoryNotificationCategory, FInventoryNotificationStyle> NotificationCategoryStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notifications|Config", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TSet<FString> NotificationTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notifications|Config", meta=(ShowOnlyInnerProperties, ForceInlineRow), meta=(GetKeyOptions="GetNotificationTypes"))
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
#endif
	
};
