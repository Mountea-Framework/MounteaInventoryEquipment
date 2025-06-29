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

#include "MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction.h"

#include "Settings/TemplatesConfig/MounteaAdvancedInventoryInteractiveWidgetConfig.h"


#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction "

FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction::FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction()
{
}

FText FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction", "Mountea Advanced Interaction Widget Config");
}

FColor FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryInteractiveWidgetConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("666. Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
