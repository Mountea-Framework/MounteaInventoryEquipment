// Fill out your copyright notice in the Description page of Project Settings.


#include "MounteaAdvancedEquipmentSettingsConfig_AssetAction.h"

#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"


#define LOCTEXT_NAMESPACE "MounteaAdvancedEquipmentSettingsConfigAssetAction"

FMounteaAdvancedEquipmentSettingsConfig_AssetAction::FMounteaAdvancedEquipmentSettingsConfig_AssetAction()
{
}

FText FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedEquipmentSettingsConfigAssetAction", "Mountea Advanced Equipment Config");
}

FColor FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedEquipmentSettingsConfig::StaticClass();
}

uint32 FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedEquipmentSettingsConfig_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("3. Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
