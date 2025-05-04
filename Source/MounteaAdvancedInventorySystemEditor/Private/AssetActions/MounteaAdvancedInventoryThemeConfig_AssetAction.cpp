// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventoryThemeConfig_AssetAction.h"

#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryThemeConfigAssetAction"

FMounteaAdvancedInventoryThemeConfig_AssetAction::FMounteaAdvancedInventoryThemeConfig_AssetAction()
{
}

FText FMounteaAdvancedInventoryThemeConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryThemeConfigAssetAction", "Mountea Advanced Inventory Theme Config");
}

FColor FMounteaAdvancedInventoryThemeConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventoryThemeConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryThemeConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryThemeConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryThemeConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
