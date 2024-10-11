// All rights reserved Dominik Morse (Pavlicek) 2024

#include "FMounteaInventoryContentThemeAssetAction.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Settings/ContentTheme.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryContentThemeAssetAction"

FMounteaInventoryContentThemeAssetAction::FMounteaInventoryContentThemeAssetAction()
{}

FText FMounteaInventoryContentThemeAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryThemeAssetAction_Name", "Inventory Content Theme");
}

FColor FMounteaInventoryContentThemeAssetAction::GetTypeColor() const
{
	return FColor::Red;
}

UClass* FMounteaInventoryContentThemeAssetAction::GetSupportedClass() const
{
	return UContentTheme::StaticClass();
}

uint32 FMounteaInventoryContentThemeAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryContentThemeAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		AdvancedMenuSubCategoryName_05
	};
	return AssetTypeActionSubMenu;
}

FText FTextThemeAssetActions::GetName() const
{
	return  LOCTEXT("TextThemeAssetAction_Name", "Text Theme");
}

UClass* FTextThemeAssetActions::GetSupportedClass() const
{
	return UTextTheme::StaticClass();
}

FText FCategoryThemeAssetActions::GetName() const
{
	return  LOCTEXT("CategoryThemeAssetAction_Name", "Category Theme");
}

UClass* FCategoryThemeAssetActions::GetSupportedClass() const
{
	return UCategoryTheme::StaticClass();
}

#undef LOCTEXT_NAMESPACE
