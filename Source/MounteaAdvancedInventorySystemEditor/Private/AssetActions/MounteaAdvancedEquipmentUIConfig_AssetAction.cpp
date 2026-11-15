// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "MounteaAdvancedEquipmentUIConfig_AssetAction.h"

#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedEquipmentUIConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedEquipmentUIConfig_AssetAction"

FMounteaAdvancedEquipmentUIConfig_AssetAction::FMounteaAdvancedEquipmentUIConfig_AssetAction()
{
}

FText FMounteaAdvancedEquipmentUIConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedEquipmentUIConfig_AssetAction", "Mountea Advanced Equipment UI Config");
}

FColor FMounteaAdvancedEquipmentUIConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedEquipmentUIConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedEquipmentUIConfig::StaticClass();
}

uint32 FMounteaAdvancedEquipmentUIConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedEquipmentUIConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> assetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::ConfigurationMenuEntry
	};

	return assetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
