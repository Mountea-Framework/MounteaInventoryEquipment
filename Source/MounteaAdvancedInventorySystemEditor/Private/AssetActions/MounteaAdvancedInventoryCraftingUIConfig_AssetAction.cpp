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

#include "MounteaAdvancedInventoryCraftingUIConfig_AssetAction.h"

#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedCraftingUIConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryCraftingUIConfig_AssetAction"

FMounteaAdvancedInventoryCraftingUIConfig_AssetAction::FMounteaAdvancedInventoryCraftingUIConfig_AssetAction()
{
}

FText FMounteaAdvancedInventoryCraftingUIConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryCraftingUIConfig_AssetAction", "Mountea Advanced Crafting UI Config");
}

FColor FMounteaAdvancedInventoryCraftingUIConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryCraftingUIConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedCraftingUIConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryCraftingUIConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryCraftingUIConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::CraftingClassesMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
