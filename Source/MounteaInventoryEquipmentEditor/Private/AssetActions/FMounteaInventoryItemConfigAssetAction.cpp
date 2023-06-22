// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemConfigAssetAction.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Setup/MounteaInventoryItemConfig.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemConfigAssetAction"

FMounteaInventoryItemConfigAssetAction::FMounteaInventoryItemConfigAssetAction()
{}

FText FMounteaInventoryItemConfigAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemConfigAssetAction_Name", "Item Config");
}

FColor FMounteaInventoryItemConfigAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaInventoryItemConfigAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemConfig::StaticClass();
}

uint32 FMounteaInventoryItemConfigAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryItemConfigAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		LOCTEXT("MounteaInventoryEquipment_Sub02", "2. Configuration")
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
