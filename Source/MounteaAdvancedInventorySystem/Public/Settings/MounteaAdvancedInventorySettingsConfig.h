// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
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
* @see [MounteaAdvancedInventorySettingsConfig](https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Settings-Config)
*/
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining inventory types, rarities and categories."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsConfig();

public:

	UPROPERTY(EditAnywhere, Category="Types", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault)) //, ReadOnlyKeys))
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	UPROPERTY(EditAnywhere, Category="Rarities", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault)) //, ReadOnlyKeys))
	TMap<FString, FInventoryRarity> AllowedRarities;

	UPROPERTY(EditAnywhere, Category="Categories", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault)) //, ReadOnlyKeys))
	TMap<FString, FInventoryCategory> AllowedCategories;

	UPROPERTY(EditAnywhere, Category="Limits", meta=(ShowOnlyInnerProperties, ForceInlineRow, NoResetToDefault))
	float InventoryBaseWeightLimit = 150.f;

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
