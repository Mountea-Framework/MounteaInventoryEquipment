// All rights reserved Dominik Morse (Pavlicek) 2024

#include "FMounteaEquipmentComponentAssetAction.h"

#include "Components/MounteaEquipmentComponent.h"
#include "Components/MounteaInventoryComponent.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"

#define LOCTEXT_NAMESPACE "MounteaEquipmentComponentAssetAction"

FMounteaEquipmentComponentAssetAction::FMounteaEquipmentComponentAssetAction()
{}

FText FMounteaEquipmentComponentAssetAction::GetName() const
{
	return LOCTEXT("MounteaEquipmentComponentAssetAction_Name", "Equipment Component");
}

FColor FMounteaEquipmentComponentAssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FMounteaEquipmentComponentAssetAction::GetSupportedClass() const
{
	return UMounteaEquipmentComponent::StaticClass();
}

uint32 FMounteaEquipmentComponentAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaEquipmentComponentAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_04
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
