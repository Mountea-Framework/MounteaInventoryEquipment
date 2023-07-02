// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemAdditionalDataAssetAction.h"
#include "Definitions/MounteaItemAdditionalData.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemAdditionalDataAssetAction"

FMounteaInventoryItemAdditionalDataAssetAction::FMounteaInventoryItemAdditionalDataAssetAction()
{}

FText FMounteaInventoryItemAdditionalDataAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemAdditionalDataAssetAction_Name", "Inventory Item Additional Data");
}

FColor FMounteaInventoryItemAdditionalDataAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaInventoryItemAdditionalDataAssetAction::GetSupportedClass() const
{
	return UMounteaItemAdditionalData::StaticClass();
}

uint32 FMounteaInventoryItemAdditionalDataAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryItemAdditionalDataAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_01
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
