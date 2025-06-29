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

TSoftObjectPtr<UInputMappingContext> UMounteaAdvancedInventorySettings::
GetAdvancedInventoryEquipmentInputMapping() const
{
	return AdvancedInventoryEquipmentInputMapping;
}
