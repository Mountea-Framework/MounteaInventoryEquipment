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
#include "Definitions/MounteaRecipeIngredientsList.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedRecipeIngredientsListAssetAction"

class  FMounteaAdvancedInventoryRecipeIngredientsList_AssetAction : public FAssetTypeActions_Base
{
public:

	virtual FText GetName() const override
	{
		return LOCTEXT("MounteaAdvancedRecipeIngredientsListAssetAction", "Mountea Recipe Ingredients List");
	};
	virtual FColor GetTypeColor() const override
	{
		return FColor::White;
	};
	virtual UClass* GetSupportedClass() const override
	{
		return UMounteaRecipeIngredientsList::StaticClass();
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