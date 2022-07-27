#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(ActorInventoryPluginEditor, All, All);

class FActorInventoryPluginEditor : public IModuleInterface
{
public:
	
	FActorInventoryPluginEditor();
	
	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:
	
	TSharedPtr<class FInventoryCategoryAssetActions> InventoryCategoryAssetActions;
};