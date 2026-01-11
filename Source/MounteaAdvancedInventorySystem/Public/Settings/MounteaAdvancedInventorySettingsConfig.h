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
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventorySettingsConfig.generated.h"

class UMounteaAdvancedInventoryThemeConfig;
class UCommonActivatableWidget;

/**
 * UMounteaAdvancedInventorySettingsConfig is a data asset managing comprehensive inventory system configuration.
 * Settings config defines inventory types, item rarities, categories, UI classes, notification systems,
 * and visual parameters for complete inventory system customization and behavior control.
 *
 * @see [Settings Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see FInventoryTypeConfig
 * @see FInventoryRarity
 * @see FInventoryCategory
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining inventory types, rarities and categories."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsConfig();

public:

	// --- Types ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="⚜ Types", meta=(ForceInlineRow, NoResetToDefault))
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	// --- Rarities ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="💎 Rarities", meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryRarity> AllowedRarities;

	// --- Categories ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="📚 Categories", meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryCategory> AllowedCategories;

	// --- Limits ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="♾ Limits", meta=(NoResetToDefault))
	float InventoryBaseWeightLimit = 150.f;

	// --- Templates ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🏗 Templates", meta=(NoResetToDefault))
	TMap<FString,TSoftObjectPtr<UPrimaryDataAsset>> TemplatesConfig;
	
	// --- Notifications
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🔔 Notifications|Config", 
		meta=(NoResetToDefault))
	TSet<FString> NotificationTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🔔 Notifications|Config", 
		meta=(ForceInlineRow), 
		meta=(GetKeyOptions="GetNotificationTypes"))
	TMap<FString, FInventoryNotificationConfig> NotificationConfigs;

	// --- User Interface ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Theme")
	TSoftObjectPtr<UMounteaAdvancedInventoryThemeConfig> BaseTheme;
	
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
	UFUNCTION(CallInEditor, Category="⚙ Default Values")
	void SetDefaultValues();

	UFUNCTION(CallInEditor, Category="⚙ Default Values")
	void SetDefaultNotificationConfig();
protected:
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
