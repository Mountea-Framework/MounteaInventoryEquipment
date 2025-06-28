// Fill out your copyright notice in the Description page of Project Settings.


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