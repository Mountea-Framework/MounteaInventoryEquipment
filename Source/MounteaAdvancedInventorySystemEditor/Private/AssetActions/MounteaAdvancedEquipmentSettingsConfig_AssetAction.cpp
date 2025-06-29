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


#include "MounteaAdvancedEquipmentSettingsConfig_AssetAction.h"

#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"


#define LOCTEXT_NAMESPACE "MounteaAdvancedEquipmentSettingsConfigAssetAction"

FMounteaAdvancedEquipmentSettingsConfig_AssetAction::FMounteaAdvancedEquipmentSettingsConfig_AssetAction()
{
}

FText FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedEquipmentSettingsConfigAssetAction", "Mountea Advanced Equipment Config");
}

FColor FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedEquipmentSettingsConfig::StaticClass();
}

uint32 FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("666. Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
