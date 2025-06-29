// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "MounteaAdvancedInventoryUIComponent_AssetAction.h"

#include "Components/MounteaInventoryUIComponent.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryUIComponent_AssetAction"

FMounteaAdvancedInventoryUIComponent_AssetAction::FMounteaAdvancedInventoryUIComponent_AssetAction()
{}

FText FMounteaAdvancedInventoryUIComponent_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryUIComponent_AssetAction_Name", "Mountea Inventory UI Component");
}

FColor FMounteaAdvancedInventoryUIComponent_AssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaAdvancedInventoryUIComponent_AssetAction::GetSupportedClass() const
{
	return UMounteaInventoryUIComponent::StaticClass();
}

uint32 FMounteaAdvancedInventoryUIComponent_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryUIComponent_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("2. Base Classes (Inventory)")
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE