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


#include "MounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction.h"
#include "Decorations/MounteaCallbackInventoryItemAction.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryItemCallbackAction"

FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction::FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction()
{}

FText FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryItemCallbackAction_Name", "Mountea Inventory Callback Item Action");
}

FColor FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction::GetSupportedClass() const
{
	return UMounteaCallbackInventoryItemAction::StaticClass();
}

uint32 FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::AssetActionsMenuEntry
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE