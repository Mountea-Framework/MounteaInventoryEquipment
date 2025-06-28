// Fill out your copyright notice in the Description page of Project Settings.


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