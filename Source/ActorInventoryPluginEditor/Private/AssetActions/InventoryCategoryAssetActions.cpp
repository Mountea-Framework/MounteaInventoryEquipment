// All rights reserved Dominik Pavlicek 2022.


#include "AssetActions/InventoryCategoryAssetActions.h"

#include "ActorInventoryPlugin/Public/Definitions/InventoryCategory.h"

UClass* FInventoryCategoryAssetActions::GetSupportedClass() const
{
	return UInventoryCategory::StaticClass();
}

FText FInventoryCategoryAssetActions::GetName() const
{
	return INVTEXT("Inventory Category");
}

FColor FInventoryCategoryAssetActions::GetTypeColor() const
{
	return FColor::Emerald;
}

uint32 FInventoryCategoryAssetActions::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Inventory"));
	}
	else
	{
		return  EAssetTypeCategories::Blueprint;
	}
}
