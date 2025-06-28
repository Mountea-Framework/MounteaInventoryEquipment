// All rights reserved Dominik Morse 2024

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
		FText::FromString("3. Configuration")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
