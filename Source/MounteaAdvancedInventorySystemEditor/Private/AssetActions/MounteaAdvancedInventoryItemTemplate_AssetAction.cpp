// Fill out your copyright notice in the Description page of Project Settings.


#include "MounteaAdvancedInventoryItemTemplate_AssetAction.h"

#include "Definitions/MounteaInventoryItemTemplate.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction "

FMounteaAdvancedInventoryItemTemplate_AssetAction::FMounteaAdvancedInventoryItemTemplate_AssetAction()
{
}

FText FMounteaAdvancedInventoryItemTemplate_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryItemTemplate_AssetAction", "Inventory Item Template");
}

FColor FMounteaAdvancedInventoryItemTemplate_AssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FMounteaAdvancedInventoryItemTemplate_AssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemTemplate::StaticClass();
}

uint32 FMounteaAdvancedInventoryItemTemplate_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryItemTemplate_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		FText::FromString("1. Templates")
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
