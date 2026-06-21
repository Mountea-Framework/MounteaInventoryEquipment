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

#include "MounteaAdvancedInventoryGlobalConfig_AssetAction.h"

#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryGlobalConfig_AssetAction"

FMounteaAdvancedInventoryGlobalConfig_AssetAction::FMounteaAdvancedInventoryGlobalConfig_AssetAction()
{
}

FText FMounteaAdvancedInventoryGlobalConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryGlobalConfig_AssetAction", "Mountea Advanced Inventory Global Config");
}

FColor FMounteaAdvancedInventoryGlobalConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryGlobalConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryGlobalConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryGlobalConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryGlobalConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
