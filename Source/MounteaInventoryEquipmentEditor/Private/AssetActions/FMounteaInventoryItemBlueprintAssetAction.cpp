// All rights reserved Dominik Morse (Pavlicek) 2024

#include "FMounteaInventoryItemBlueprintAssetAction.h"

#include "Definitions/MounteaInventoryItemBlueprint.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"

#define LOCTEXT_NAMESPACE "FMounteaInventoryItemBlueprintAssetAction"

FMounteaInventoryItemBlueprintAssetAction::FMounteaInventoryItemBlueprintAssetAction()
{
}

FText FMounteaInventoryItemBlueprintAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemBlueprintAssetAction_Name", "Item Blueprint");
}

FColor FMounteaInventoryItemBlueprintAssetAction::GetTypeColor() const
{
	return FColor::Yellow;
}

UClass* FMounteaInventoryItemBlueprintAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemBlueprint::StaticClass();
}

uint32 FMounteaInventoryItemBlueprintAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryItemBlueprintAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_02
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE