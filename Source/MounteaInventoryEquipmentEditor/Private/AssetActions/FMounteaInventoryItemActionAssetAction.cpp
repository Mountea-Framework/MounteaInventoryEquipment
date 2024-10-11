// All rights reserved Dominik Morse (Pavlicek) 2024

#include "FMounteaInventoryItemActionAssetAction.h"

#include "Definitions/MounteaItemAction.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"



#define LOCTEXT_NAMESPACE "MounteaInventoryItemActionAssetAction"

FMounteaInventoryItemActionAssetAction::FMounteaInventoryItemActionAssetAction()
{}

FText FMounteaInventoryItemActionAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemActionAssetAction_Name", "Item Action (Base)");
}

FColor FMounteaInventoryItemActionAssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FMounteaInventoryItemActionAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemAction::StaticClass();
}

uint32 FMounteaInventoryItemActionAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryItemActionAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_01
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
