#pragma once

#include "Modules/ModuleManager.h"

class FSlateStyleSet;

DECLARE_LOG_CATEGORY_EXTERN(ActorInventoryPluginEditor, All, All);

class FActorInventoryPluginEditor : public IModuleInterface
{
public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	TSharedPtr<FSlateStyleSet> InventoryComponentSet;
	TSharedPtr<FSlateStyleSet> InventoryManagerSet;
	TSharedPtr<FSlateStyleSet> InventoryCategorySet;
	TSharedPtr<FSlateStyleSet> InventoryRaritySet;
	TSharedPtr<FSlateStyleSet> InventoryKeyActionsSet;
	TSharedPtr<FSlateStyleSet> InventoryItemSet;
	TSharedPtr<FSlateStyleSet> InventoryItemComponentSet;
	
	TSharedPtr<class FInventoryCategoryAssetActions> InventoryCategoryAssetActions;
	TSharedPtr<class FInventoryRarityAssetActions> InventoryRarityAssetsActions;
	TSharedPtr<class FInventoryKeyActionAssetActions> InventoryKeyActionAssetActions;
	
	TSharedPtr<class FInventoryItemAssetActions> InventoryItemAssetActions;
	
};
