// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryThemeAssetAction.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Settings/MounteaInventoryThemeConfig.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryThemeAssetAction"

FMounteaInventoryThemeAssetAction::FMounteaInventoryThemeAssetAction()
{}

FText FMounteaInventoryThemeAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryThemeAssetAction_Name", "2. Inventory Theme Config");
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
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryNameData);
	}
	
	return  EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE
