// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"
#include "Definitions/InventoryCategory.h"

class FInventoryCategoryAssetActions : public FAssetTypeActions_Base
{
	
public:
	virtual UClass* GetSupportedClass() const override
	{
		return UInventoryCategory::StaticClass();
	};
	virtual FText GetName() const override
	{
		return INVTEXT("Inventory Category");
	};
	virtual FColor GetTypeColor() const override
	{
		return FColor::Emerald;
	};
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Inventory"));
		}
	
		return  EAssetTypeCategories::Misc;
	};
};
