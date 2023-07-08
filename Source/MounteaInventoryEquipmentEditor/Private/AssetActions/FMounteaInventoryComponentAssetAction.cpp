// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryComponentAssetAction.h"
#include "Components/MounteaInventoryComponent.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryComponentAssetAction"

FMounteaInventoryComponentAssetAction::FMounteaInventoryComponentAssetAction()
{}

FText FMounteaInventoryComponentAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryComponentAssetAction_Name", "Inventory Component");
}

FColor FMounteaInventoryComponentAssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FMounteaInventoryComponentAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryComponent::StaticClass();
}

uint32 FMounteaInventoryComponentAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryComponentAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_04
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
