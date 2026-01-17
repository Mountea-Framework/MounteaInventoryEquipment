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


#include "MounteaAdvancedInventorySelectableInventoryItemAction_AssetAction.h"
#include "Decorations/MounteaSelectableInventoryItemAction.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryItemSelectableAction"

FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction::FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction()
{}

FText FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryItemSelectableAction_Name", "Mountea Inventory Selectable Item Action");
}

FColor FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction::GetSupportedClass() const
{
	return UMounteaSelectableInventoryItemAction::StaticClass();
}

uint32 FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("4. Item Actions")
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE