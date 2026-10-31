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

#include "MounteaAdvancedInventoryCraftingAction_AssetAction.h"

#include "Decorations/MounteaCraftingAction.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryCraftingAction"

FMounteaAdvancedInventoryCraftingAction_AssetAction::FMounteaAdvancedInventoryCraftingAction_AssetAction()
{
}

FText FMounteaAdvancedInventoryCraftingAction_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryCraftingAction_Name", "Mountea Crafting Action");
}

FColor FMounteaAdvancedInventoryCraftingAction_AssetAction::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FMounteaAdvancedInventoryCraftingAction_AssetAction::GetSupportedClass() const
{
	return UMounteaCraftingAction::StaticClass();
}

uint32 FMounteaAdvancedInventoryCraftingAction_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryCraftingAction_AssetAction::GetSubMenus() const
{
	static const TArray<FText> assetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::AssetActionsMenuEntry
	};
	return assetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
