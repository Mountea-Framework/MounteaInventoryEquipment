// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Helpers/InventoryHelpers.h"

class FInventoryTransactionAssetActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override
	{
		return UInventoryTransaction::StaticClass();
	}
	;
	virtual FText GetName() const override
	{
		return INVTEXT("Inventory Transaction");
	};
	virtual FColor GetTypeColor() const override
	{
		return FColor::Black;
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
