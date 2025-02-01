// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "Logs/MounteaAdvancedInventorySystemLogTypes.h"
#include "MounteaAdvancedInventorySettings.generated.h"

class UMounteaAdvancedInventorySettingsConfig;

/**
 * 
 */
UCLASS(config = MounteaSettings, defaultconfig, meta = (DisplayName = "Mountea Inventory System Settings"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettings();

public:

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaAdvancedInventorySystem", "MounteaSettingsDescription", "Mountea Inventory System");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaAdvancedInventorySystem", "MounteaSettingsDescription", "Default values for Mountea Advanced Inventory Plugin.");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}
#endif

public:

	UPROPERTY(Config, EditAnywhere, Category="Rarity")
	TSoftObjectPtr<UMounteaAdvancedInventorySettingsConfig> InventorySettingsConfig;

	/**
	 * Defines logging level that is allowed to be shown.
	 * Affects on-screen messages.
	 */
	UPROPERTY(config, EditDefaultsOnly, Category = "Logging", meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EMounteaAdvancedInventoryLoggingVerbosity"))
	uint8 LogVerbosity;

public:

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	EMounteaAdvancedInventoryLoggingVerbosity GetAllowedLoggVerbosity() const;

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	TArray<FString> GetAllowedRarities() const;
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	TArray<FString> GetAllowedCategories() const;
};
