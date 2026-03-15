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


#include "MounteaAdvancedInventoryLoadoutItem_AssetAction.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutItem.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryItemSelectableAction"

FMounteaAdvancedInventoryLoadoutItem_AssetAction::FMounteaAdvancedInventoryLoadoutItem_AssetAction()
{}

FText FMounteaAdvancedInventoryLoadoutItem_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryItemSelectableAction_Name", "Mountea Inventory Loadout Item");
}

FColor FMounteaAdvancedInventoryLoadoutItem_AssetAction::GetTypeColor() const
{
	return FColor::White;
}

UClass* FMounteaAdvancedInventoryLoadoutItem_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryLoadoutItem::StaticClass();
}

uint32 FMounteaAdvancedInventoryLoadoutItem_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryLoadoutItem_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::LoadoutClassesMenuEntry
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE