// All rights reserved Dominik Morse 2024


#include "Settings/MounteaAdvancedInventorySettings.h"

#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

UMounteaAdvancedInventorySettings::UMounteaAdvancedInventorySettings() : InventorySettingsConfig(nullptr),
																		 LogVerbosity(14)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory System");
}

EMounteaAdvancedInventoryLoggingVerbosity UMounteaAdvancedInventorySettings::GetAllowedLoggVerbosity() const
{
	return static_cast<EMounteaAdvancedInventoryLoggingVerbosity>(LogVerbosity);
}

TArray<FString> UMounteaAdvancedInventorySettings::GetAllowedRarities() const
{
	TArray<FString> returnValues;
	auto inventorySettingsConfig = InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
	{
		returnValues.Add(TEXT("Common"));
		return returnValues;
	}
	inventorySettingsConfig->AllowedRarities.GetKeys(returnValues);
	return returnValues;
}

TArray<FString> UMounteaAdvancedInventorySettings::GetAllowedCategories() const
{
	TArray<FString> returnValues;
	auto inventorySettingsConfig = InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
	{
		returnValues.Add(TEXT("Miscellaneous"));
		return returnValues;
	}
	inventorySettingsConfig->AllowedCategories.GetKeys(returnValues);
	return returnValues;
}
