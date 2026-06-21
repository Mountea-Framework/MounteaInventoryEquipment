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

#include "MounteaAdvancedInventoryGlobalConfig_Factory.h"

#include "AssetToolsModule.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryGlobalConfig_Factory"

UMounteaAdvancedInventoryGlobalConfig_Factory::UMounteaAdvancedInventoryGlobalConfig_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryGlobalConfig::StaticClass();
	ParentClass = UMounteaAdvancedInventoryGlobalConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryGlobalConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryGlobalConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryGlobalConfig_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Global Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryGlobalConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

FText UMounteaAdvancedInventoryGlobalConfig_Factory::GetDisplayName() const
{
	return LOCTEXT("MounteaAdvancedInventoryGlobalConfig_Factory_DisplayName", "Mountea Advanced Inventory Global Config");
}

FString UMounteaAdvancedInventoryGlobalConfig_Factory::GetDefaultNewAssetName() const
{
	return TEXT("NewMounteaAdvancedInventoryGlobalConfig");
}

uint32 UMounteaAdvancedInventoryGlobalConfig_Factory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaAdvancedInventoryGlobalConfig_Factory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
