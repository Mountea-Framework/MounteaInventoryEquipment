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

#include "MounteaAdvancedInventorySettingsConfig_AssetAction.h"

#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventorySettingsConfigAssetAction"

FMounteaAdvancedInventorySettingsConfig_AssetAction::FMounteaAdvancedInventorySettingsConfig_AssetAction()
{
}

FText FMounteaAdvancedInventorySettingsConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventorySettingsConfigAssetAction", "Mountea Advanced Inventory Config");
}

FColor FMounteaAdvancedInventorySettingsConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventorySettingsConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventorySettingsConfig::StaticClass();
}

uint32 FMounteaAdvancedInventorySettingsConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventorySettingsConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("666. Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
