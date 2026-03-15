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

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Definitions/MounteaRecipeTemplate.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedRecipeTemplateAssetAction"

class  FMounteaAdvancedInventoryRecipeTemplate_AssetAction : public FAssetTypeActions_Base
{
public:

	virtual FText GetName() const override
	{
		return LOCTEXT("MounteaAdvancedRecipeTemplateAssetAction", "Mountea Recipe Template");
	};
	virtual FColor GetTypeColor() const override
	{
		return FColor::Emerald;
	};
	virtual UClass* GetSupportedClass() const override
	{
		return UMounteaRecipeTemplate::StaticClass();
	};
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
		}

		return EAssetTypeCategories::Misc;
	};
	virtual const TArray<FText>& GetSubMenus() const override
	{
		static const TArray<FText> AssetTypeActionSubMenu
		{
			MounteaAdvancedInventoryBaseTypes::CraftingClassesMenuEntry
		};
		return AssetTypeActionSubMenu;
	};
};

#undef LOCTEXT_NAMESPACE