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

#include "MounteaAdvancedInventoryLoadoutConfigs_AssetAction.h"

#include "MounteaAdvancedInventoryThemeConfig_AssetAction.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"


#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryLoadoutConfigs_AssetAction"

FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::FMounteaAdvancedInventoryLoadoutConfigs_AssetAction()
{
}

FText FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryLoadoutConfigs_AssetAction", "Mountea Advanced Inventory Loadout Config");
}

FColor FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryLoadoutConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
