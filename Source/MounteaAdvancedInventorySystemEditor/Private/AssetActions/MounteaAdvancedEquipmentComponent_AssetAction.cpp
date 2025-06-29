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


#include "MounteaAdvancedEquipmentComponent_AssetAction.h"

#include "Components/MounteaEquipmentComponent.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedEquipmentComponent_AssetAction"

FMounteaAdvancedEquipmentComponent_AssetAction::FMounteaAdvancedEquipmentComponent_AssetAction()
{}

FText FMounteaAdvancedEquipmentComponent_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedEquipmentComponent_AssetAction_Name", "Mountea Equipment Component");
}

FColor FMounteaAdvancedEquipmentComponent_AssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaAdvancedEquipmentComponent_AssetAction::GetSupportedClass() const
{
	return UMounteaEquipmentComponent::StaticClass();
}

uint32 FMounteaAdvancedEquipmentComponent_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedEquipmentComponent_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("3. Base Classes (Equipment)")
	};
	return AssetTypeActionSubMenu;
};

#undef LOCTEXT_NAMESPACE