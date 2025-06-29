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


#include "MounteaAdvancedInventoryComponent_AssetAction.h"

#include "Components/MounteaInventoryComponent.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryComponent_AssetAction"

FMounteaAdvancedInventoryComponent_AssetAction::FMounteaAdvancedInventoryComponent_AssetAction()
{}

FText FMounteaAdvancedInventoryComponent_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryComponent_AssetAction_Name", "Mountea Inventory Component");
}

FColor FMounteaAdvancedInventoryComponent_AssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaAdvancedInventoryComponent_AssetAction::GetSupportedClass() const
{
	return UMounteaInventoryComponent::StaticClass();
}

uint32 FMounteaAdvancedInventoryComponent_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryComponent_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("2. Base Classes (Inventory)")
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE