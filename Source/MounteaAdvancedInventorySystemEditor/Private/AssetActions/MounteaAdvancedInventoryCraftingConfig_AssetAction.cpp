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

#include "MounteaAdvancedInventoryCraftingConfig_AssetAction.h"

#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedCraftingConfig.h"


#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryCraftingConfig_AssetAction "

FMounteaAdvancedInventoryCraftingConfig_AssetAction::FMounteaAdvancedInventoryCraftingConfig_AssetAction()
{
}

FText FMounteaAdvancedInventoryCraftingConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryCraftingConfig_AssetAction", "Mountea Advanced Crafting Config");
}

FColor FMounteaAdvancedInventoryCraftingConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryCraftingConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedCraftingConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryCraftingConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryCraftingConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::CraftingClassesMenuEntry
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
