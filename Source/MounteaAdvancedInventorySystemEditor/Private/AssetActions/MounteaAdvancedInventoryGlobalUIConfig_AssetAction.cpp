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

#include "MounteaAdvancedInventoryGlobalUIConfig_AssetAction.h"

#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryGlobalUIConfig_AssetAction"

FMounteaAdvancedInventoryGlobalUIConfig_AssetAction::FMounteaAdvancedInventoryGlobalUIConfig_AssetAction()
{
}

FText FMounteaAdvancedInventoryGlobalUIConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryGlobalUIConfig_AssetAction", "Mountea Advanced Inventory Global UI Config");
}

FColor FMounteaAdvancedInventoryGlobalUIConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryGlobalUIConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryGlobalUIConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryGlobalUIConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryGlobalUIConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
