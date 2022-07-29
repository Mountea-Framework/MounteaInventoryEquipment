// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Definitions/InventoryItemRarity.h"

class FInventoryRarityAssetActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override
	{
		return UInventoryItemRarity::StaticClass();
	}
	;
	virtual FText GetName() const override
	{
		return INVTEXT("Inventory Rarity");
	};
	virtual FColor GetTypeColor() const override
	{
		return FColor::Cyan;
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
