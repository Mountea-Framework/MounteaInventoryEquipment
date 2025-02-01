// All rights reserved Dominik Morse 2024

#pragma once

#include "Modules/ModuleManager.h"

class FMounteaAdvancedInventorySystemEditor : public IModuleInterface
{
public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	TSharedPtr<FSlateStyleSet> AdvancedInventorySet;
	TArray<TSharedPtr<class FAssetTypeActions_Base>> AssetActions;
};