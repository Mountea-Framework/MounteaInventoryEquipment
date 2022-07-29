﻿// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Definitions/InventoryItem.h"

class FInventoryItemAssetActions : public FAssetTypeActions_Base
{
	
public:
	
	virtual UClass* GetSupportedClass() const override {return UInventoryItem::StaticClass(); };
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_Blueprint", "Inventory Item Object"); }
	virtual FColor GetTypeColor() const override { return FColor::Yellow; };
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Inventory"));
		}
	
		return  EAssetTypeCategories::Misc;
	};
};
