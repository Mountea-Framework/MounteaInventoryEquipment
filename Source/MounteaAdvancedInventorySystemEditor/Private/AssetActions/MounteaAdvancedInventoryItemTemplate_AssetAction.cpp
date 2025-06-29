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


#include "MounteaAdvancedInventoryItemTemplate_AssetAction.h"

#include "Definitions/MounteaInventoryItemTemplate.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction "

FMounteaAdvancedInventoryItemTemplate_AssetAction::FMounteaAdvancedInventoryItemTemplate_AssetAction()
{
}

FText FMounteaAdvancedInventoryItemTemplate_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryItemTemplate_AssetAction", "Inventory Item Template");
}

FColor FMounteaAdvancedInventoryItemTemplate_AssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FMounteaAdvancedInventoryItemTemplate_AssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemTemplate::StaticClass();
}

uint32 FMounteaAdvancedInventoryItemTemplate_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryItemTemplate_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("1. Templates")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
