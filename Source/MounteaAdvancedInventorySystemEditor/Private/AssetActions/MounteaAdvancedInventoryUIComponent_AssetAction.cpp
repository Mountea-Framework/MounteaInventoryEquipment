// Fill out your copyright notice in the Description page of Project Settings.


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