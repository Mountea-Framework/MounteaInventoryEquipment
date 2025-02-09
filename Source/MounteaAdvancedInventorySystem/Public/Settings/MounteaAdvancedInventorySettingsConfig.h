// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventorySettingsConfig.generated.h"

USTRUCT(BlueprintType)
struct FInventoryNotificationStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FLinearColor BackgroundColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FLinearColor IconTint = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct FInventoryNotificationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DefaultDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bShowProgressBar = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector2D NotificationSize = FVector2D(400.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FInventoryNotificationStyle CategoryStyle;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSoftObjectPtr<USoundBase> NotificationSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText MessageTemplate;
};

/**
* Data Asset that manages configuration settings for Mountea Advanced Inventory System.
* Defines inventory types, rarities, and categories with their respective settings.
*
* @see UDataAsset
* @see FInventoryTypeConfig
* @see FInventoryRarity
* @see FInventoryCategory
* @see [MounteaAdvancedInventorySettingsConfig](https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Settings-Config)
*/
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining inventory types, rarities and categories."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsConfig();

public:

	UPROPERTY(EditAnywhere, Category="Types", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	UPROPERTY(EditAnywhere, Category="Rarities", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryRarity> AllowedRarities;

	UPROPERTY(EditAnywhere, Category="Categories", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryCategory> AllowedCategories;

	UPROPERTY(EditAnywhere, Category="Limits", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	float InventoryBaseWeightLimit = 150.f;

	UPROPERTY(EditAnywhere, Category="Notifications", meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetClass;
	
	UPROPERTY(EditAnywhere, Category="Notifications", meta=(ShowOnlyInnerProperties, ForceInlineRow))
	TMap<EInventoryNotificationCategory, FInventoryNotificationConfig> NotificationConfigs;
	
protected:

	void ValidateInventoryTypes();
	static FInventoryTypeConfig GetDefaultConfigForType(const EInventoryType Type);

	static void SetupPlayerConfig(FInventoryTypeConfig& Config);
	static void SetupNPCConfig(FInventoryTypeConfig& Config);
	static void SetupStorageConfig(FInventoryTypeConfig& Config);
	static void SetupMerchantConfig(FInventoryTypeConfig& Config);
	static void SetupLootConfig(FInventoryTypeConfig& Config);
	static void SetupSpecializedConfig(FInventoryTypeConfig& Config);

#if WITH_EDITOR
public:
	/** Sets default values. Default values can be edited. If requested then Default values will override all `Categories` and `Rarities`! */
	UFUNCTION(CallInEditor, Category="Default Values")
	void SetDefaultValues();
protected:
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	
};
