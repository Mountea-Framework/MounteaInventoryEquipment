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

#include "Subsystems/MounteaEditorDefaultsSubsystem.h"

#include "InputMappingContext.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Settings/MounteaAdvancedCraftingConfig.h"
#include "Settings/MounteaAdvancedCraftingUIConfig.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Settings/MounteaAdvancedInventoryUIConfig.h"

void UMounteaEditorDefaultsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ApplyDefaultsIfNeeded();
}

void UMounteaEditorDefaultsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMounteaEditorDefaultsSubsystem::ApplyDefaultsIfNeeded() const
{
	UMounteaAdvancedInventorySettings* settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	if (!settings)
	{
		LOG_ERROR(TEXT("[Editor Defaults] Failed to access inventory settings."));
		return;
	}

	bool bAnyChange = false;
	bAnyChange |= TryAssignMissing(settings->AdvancedInventoryEquipmentInputMapping, InputMappingPath, TEXT("AdvancedInventoryEquipmentInputMapping"));
	bAnyChange |= TryAssignMissing(settings->AdvancedInventorySettingsConfig, InventorySettingsConfigPath, TEXT("AdvancedInventorySettingsConfig"));
	bAnyChange |= TryAssignMissing(settings->AdvancedEquipmentSettingsConfig, EquipmentSettingsConfigPath, TEXT("AdvancedEquipmentSettingsConfig"));
	bAnyChange |= TryAssignMissing(settings->AdvancedInventoryUISettingsConfig, InventoryUISettingsConfigPath, TEXT("AdvancedInventoryUISettingsConfig"));
	bAnyChange |= TryAssignMissing(settings->AdvancedCraftingSettingsConfig, CraftingSettingsConfigPath, TEXT("AdvancedCraftingSettingsConfig"));
	bAnyChange |= TryAssignMissing(settings->AdvancedCraftingUISettingsConfig, CraftingUISettingsConfigPath, TEXT("AdvancedCraftingUISettingsConfig"));

	if (bAnyChange)
	{
		settings->SaveConfig();
		LOG_INFO(TEXT("[Editor Defaults] Saved updated inventory defaults to config."));
	}
}

