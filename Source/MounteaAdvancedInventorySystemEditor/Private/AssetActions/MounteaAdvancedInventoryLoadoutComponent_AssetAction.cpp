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


#include "MounteaAdvancedInventoryLoadoutComponent_AssetAction.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Components/MounteaAdvancedInventoryLoadoutComponent.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryLoadoutComponent_AssetAction"

FMounteaAdvancedInventoryLoadoutComponent_AssetAction::FMounteaAdvancedInventoryLoadoutComponent_AssetAction()
{}

FText FMounteaAdvancedInventoryLoadoutComponent_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryLoadoutComponent_AssetAction_Name", "Mountea Inventory Loadout Component");
}

FColor FMounteaAdvancedInventoryLoadoutComponent_AssetAction::GetTypeColor() const
{
	return FColor::White;
}

UClass* FMounteaAdvancedInventoryLoadoutComponent_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryLoadoutComponent::StaticClass();
}

uint32 FMounteaAdvancedInventoryLoadoutComponent_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryLoadoutComponent_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::LoadoutClassesMenuEntry
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE
