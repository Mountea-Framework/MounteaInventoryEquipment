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

UMounteaAdvancedInventorySettings::UMounteaAdvancedInventorySettings() : AdvancedInventorySettingsConfig(nullptr),
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
	auto advancedInventorySettingsConfig = AdvancedInventorySettingsConfig.LoadSynchronous();
	if (!IsValid(advancedInventorySettingsConfig))
	{
		TMap<FString, FInventoryRarity> returnValues;
		FInventoryRarity CommonRarity;
		CommonRarity.RarityDisplayName = NSLOCTEXT(
			"UMounteaAdvancedInventorySettings", "CommonRarity", "Common");
		CommonRarity.RarityColor = FLinearColor(0.5f, 0.5f, 0.5f);
		CommonRarity.BasePriceMultiplier = 1.0f;
		CommonRarity.RarityPriority = 0;
		returnValues.Add(TEXT("Common"), CommonRarity);

		return returnValues;
	}

	auto returnValues = advancedInventorySettingsConfig->AllowedRarities;

	returnValues.ValueStableSort(
		[](const FInventoryRarity& A, const FInventoryRarity& B)
		{
			return A.RarityPriority > B.RarityPriority;
		});

	return returnValues;
}

TMap<FString, FInventoryCategory> UMounteaAdvancedInventorySettings::GetAllowedCategories() const
{
	auto advancedInventorySettingsConfig = AdvancedInventorySettingsConfig.LoadSynchronous();
	if (!IsValid(advancedInventorySettingsConfig))
	{
		TMap<FString, FInventoryCategory> returnValues;
		FInventoryCategory miscellaneousCategory;
		miscellaneousCategory.CategoryData.CategoryDisplayName = NSLOCTEXT(
			"UMounteaAdvancedInventorySettings", "miscellaneousCategory", "Miscellaneous");
		miscellaneousCategory.CategoryData.CategoryPriority = -5;
		returnValues.Add(TEXT("All"), miscellaneousCategory);

		return returnValues;
	}

	auto returnValues = advancedInventorySettingsConfig->AllowedCategories;

	returnValues.ValueStableSort(
		[](const FInventoryCategory& A, const FInventoryCategory& B)
		{
			return A.CategoryData.CategoryPriority > B.CategoryData.CategoryPriority;
		});

	return returnValues;
}


TSoftObjectPtr<UInputMappingContext> UMounteaAdvancedInventorySettings::
GetAdvancedInventoryEquipmentInputMapping() const
{
	return AdvancedInventoryEquipmentInputMapping;
}
