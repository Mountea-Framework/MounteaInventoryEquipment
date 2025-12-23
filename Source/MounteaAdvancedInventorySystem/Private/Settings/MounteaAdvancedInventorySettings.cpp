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

TMap<FString, FInventoryRarity> UMounteaAdvancedInventorySettings::GetAllowedRarities() const
{
	auto inventorySettingsConfig = InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
	{
		TMap<FString, FInventoryRarity> returnValues;
		FInventoryRarity CommonRarity;
		CommonRarity.RarityDisplayName = NSLOCTEXT(
			"UMounteaAdvancedInventorySettings", "CommonRarity", "Common");
		CommonRarity.RarityColor = FLinearColor(0.5f, 0.5f, 0.5f);
		CommonRarity.BasePriceMultiplier = 1.0f;
		CommonRarity.RarityPriority = 0;
		returnValues.Add("Common", CommonRarity);
		return returnValues;
	}
	return inventorySettingsConfig->AllowedRarities;
}

TMap<FString, FInventoryCategory> UMounteaAdvancedInventorySettings::GetAllowedCategories() const
{
	auto inventorySettingsConfig = InventorySettingsConfig.LoadSynchronous();
	if (!IsValid(inventorySettingsConfig))
	{
		TMap<FString, FInventoryCategory> returnValues;
		FInventoryCategory miscellaneousCategory;
		miscellaneousCategory.CategoryData.CategoryDisplayName = NSLOCTEXT(
			"UMounteaAdvancedInventorySettings", "miscellaneousCategory", "Miscellaneous");
		miscellaneousCategory.CategoryData.CategoryPriority = -5;
		returnValues.Add("All", miscellaneousCategory);
		return returnValues;
	}
	
	return inventorySettingsConfig->AllowedCategories;
}

TSoftObjectPtr<UInputMappingContext> UMounteaAdvancedInventorySettings::
GetAdvancedInventoryEquipmentInputMapping() const
{
	return AdvancedInventoryEquipmentInputMapping;
}
