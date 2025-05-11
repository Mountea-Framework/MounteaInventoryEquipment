// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "Logs/MounteaAdvancedInventorySystemLogTypes.h"
#include "MounteaAdvancedInventorySettings.generated.h"

class UInputMappingContext;
class UMounteaAdvancedInventorySettingsConfig;

/**
 * Represents the main configuration settings for the Mountea Advanced Inventory System.
 * This class provides centralized control over inventory-related configurations,
 * including input mappings, logging verbosity, and allowed inventory properties.
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
