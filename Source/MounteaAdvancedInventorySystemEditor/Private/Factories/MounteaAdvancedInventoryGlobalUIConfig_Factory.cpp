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

#include "MounteaAdvancedInventoryGlobalUIConfig_Factory.h"

#include "AssetToolsModule.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryGlobalUIConfig_Factory"

UMounteaAdvancedInventoryGlobalUIConfig_Factory::UMounteaAdvancedInventoryGlobalUIConfig_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryGlobalUIConfig::StaticClass();
	ParentClass = UMounteaAdvancedInventoryGlobalUIConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryGlobalUIConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryGlobalUIConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryGlobalUIConfig_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Global UI Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryGlobalUIConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

FText UMounteaAdvancedInventoryGlobalUIConfig_Factory::GetDisplayName() const
{
	return LOCTEXT("MounteaAdvancedInventoryGlobalUIConfig_Factory_DisplayName", "Mountea Advanced Inventory Global UI Config");
}

FString UMounteaAdvancedInventoryGlobalUIConfig_Factory::GetDefaultNewAssetName() const
{
	return TEXT("NewMounteaAdvancedInventoryGlobalUIConfig");
}

uint32 UMounteaAdvancedInventoryGlobalUIConfig_Factory::GetMenuCategories() const
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& UMounteaAdvancedInventoryGlobalUIConfig_Factory::GetMenuCategorySubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::ConfigurationMenuEntry
	};
	return AssetTypeActionSubMenu;
}

#undef LOCTEXT_NAMESPACE
