// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryThemeAssetAction.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Settings/MounteaInventoryThemeConfig.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryThemeAssetAction"

FMounteaInventoryThemeAssetAction::FMounteaInventoryThemeAssetAction()
{}

FText FMounteaInventoryThemeAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryThemeAssetAction_Name", "Inventory Theme Config");
}

FColor FMounteaInventoryThemeAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaInventoryThemeAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryThemeConfig::StaticClass();
}

uint32 FMounteaInventoryThemeAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryThemeAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		LOCTEXT("MounteaInventoryEquipment_Sub02", "2. Configuration")
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
