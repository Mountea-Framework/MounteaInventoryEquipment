// All rights reserved Dominik Morse (Pavlicek) 2024

#include "FMounteaInventoryCategoryAssetAction.h"

#include "Definitions/MounteaInventoryItemCategory.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryCategoryAssetAction"

FMounteaInventoryCategoryAssetAction::FMounteaInventoryCategoryAssetAction()
{}

FText FMounteaInventoryCategoryAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryCategoryAssetAction_Name", "Inventory Category Type");
}

FColor FMounteaInventoryCategoryAssetAction::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FMounteaInventoryCategoryAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemCategory::StaticClass();
}

uint32 FMounteaInventoryCategoryAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryCategoryAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_03
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
