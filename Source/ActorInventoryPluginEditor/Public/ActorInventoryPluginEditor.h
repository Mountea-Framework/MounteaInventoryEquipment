#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(ActorInventoryPluginEditor, All, All);

class FActorInventoryPluginEditor : public IModuleInterface
{
public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	
	TSharedPtr<class FInventoryCategoryAssetActions> InventoryCategoryAssetActions;
	TSharedPtr<class FInventoryRarityAssetActions> InventoryRarityAssetsActions;
	TSharedPtr<class FInventoryKeyActionAssetActions> InventoryKeyActionAssetActions;
	
	TSharedPtr<class FInventoryItemAssetActions> InventoryItemAssetActions;
	/*
	TSharedPtr<class FInventoryManagerAssetActions> InventoryManagerAssetActions;
	TSharedPtr<class FInventoryComponentAssetActions> InventoryComponentAssetActions;
	*/
	
};
