// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventorySettingsConfig_AssetAction.h"

#include "Settings/MounteaAdvancedInventorySettingsConfig.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventorySettingsConfigAssetAction"

FMounteaAdvancedInventorySettingsConfig_AssetAction::FMounteaAdvancedInventorySettingsConfig_AssetAction()
{
}

FText FMounteaAdvancedInventorySettingsConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventorySettingsConfigAssetAction", "Mountea Advanced Inventory Config");
}

FColor FMounteaAdvancedInventorySettingsConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedInventorySettingsConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventorySettingsConfig::StaticClass();
}

uint32 FMounteaAdvancedInventorySettingsConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventorySettingsConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
