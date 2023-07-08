// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryRarityAssetAction.h"

#include "Definitions/MounteaInventoryItemRarity.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryRarityAssetAction"

FMounteaInventoryRarityAssetAction::FMounteaInventoryRarityAssetAction()
{}

FText FMounteaInventoryRarityAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryRarityAssetAction_Name", "Inventory Rarity Type");
}

FColor FMounteaInventoryRarityAssetAction::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FMounteaInventoryRarityAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemRarity::StaticClass();
}

uint32 FMounteaInventoryRarityAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryRarityAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_03
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
