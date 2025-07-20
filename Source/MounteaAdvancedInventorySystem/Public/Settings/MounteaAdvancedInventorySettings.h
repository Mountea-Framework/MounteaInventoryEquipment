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
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "Logs/MounteaAdvancedInventorySystemLogTypes.h"
#include "MounteaAdvancedInventorySettings.generated.h"

class UMounteaAdvancedEquipmentSettingsConfig;
class UInputMappingContext;
class UMounteaAdvancedInventorySettingsConfig;

/**
 * UMounteaAdvancedInventorySettings provides centralized configuration for the Mountea Advanced Inventory System.
 * Settings control input mappings, logging verbosity, asset references, and system-wide behavior
 * through Unreal's developer settings framework for project-wide inventory configuration.
 *
 * @see [System Settings](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see UMounteaAdvancedInventorySettingsConfig
 * @see UDeveloperSettings
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
	
	/**
	 * Represents a soft reference to an input mapping context used specifically for managing
	 * equipment-related input within the Mountea Advanced Inventory system.
	 * This can be configured to define custom input actions for inventory equipment handling.
	 */
	UPROPERTY(Config, EditAnywhere, Category="Config")
	TSoftObjectPtr<UInputMappingContext> AdvancedInventoryEquipmentInputMapping;

	/**
	 * Holds a reference to the configuration settings for Mountea Advanced Inventory.
	 * This reference is used to define and load inventory-specific configurations.
	 */
	UPROPERTY(Config, EditAnywhere, Category="Config")
	TSoftObjectPtr<UMounteaAdvancedInventorySettingsConfig> InventorySettingsConfig;

	/**
	 * Holds a reference to the configuration settings for Mountea Advanced Equipment.
	 * This reference is used to define and load equipment-specific configurations.
	 */
	UPROPERTY(Config, EditAnywhere, Category="Config")
	TSoftObjectPtr<UMounteaAdvancedEquipmentSettingsConfig> EquipmentSettingsConfig;

	/**
	 * Defines logging level that is allowed to be shown.
	 * Affects on-screen messages.
	 */
	UPROPERTY(config, EditDefaultsOnly, Category = "Logging", meta=(Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EMounteaAdvancedInventoryLoggingVerbosity"))
	uint8 LogVerbosity;

public:
	
	/**
	 * Retrieves the allowed logging verbosity level from the Mountea Advanced Inventory settings.
	 * This level determines which messages are permitted to be displayed, helping to manage verbosity levels for logging purposes.
	 *
	 * @return The allowed logging verbosity as an enum value of EMounteaAdvancedInventoryLoggingVerbosity.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	EMounteaAdvancedInventoryLoggingVerbosity GetAllowedLoggVerbosity() const;

	/**
	 * Retrieves the list of allowed rarities for inventory items as defined in the Mountea Advanced Inventory settings.
	 * If no valid settings configuration is found, a default rarity of "Common" is returned.
	 *
	 * @return An array of strings representing the allowed item rarities.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	TArray<FString> GetAllowedRarities() const;

	/**
	 * Retrieves the list of allowed categories for inventory items as defined in the Mountea Advanced Inventory settings.
	 * If no valid settings configuration is found, a default category of "Miscellaneous" is returned.
	 *
	 * @return An array of strings representing the allowed inventory categories.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	TArray<FString> GetAllowedCategories() const;

	/**
	 * Retrieves the input mapping context used for handling equipment-related actions
	 * within the Mountea Advanced Inventory system.
	 *
	 * @return A soft object pointer to the input mapping context for inventory equipment management.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory|Settings",  meta=(CustomTag="MounteaK2Getter"))
	TSoftObjectPtr<UInputMappingContext> GetAdvancedInventoryEquipmentInputMapping() const;
};
