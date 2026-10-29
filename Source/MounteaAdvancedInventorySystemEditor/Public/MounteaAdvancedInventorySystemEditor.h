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

#include "Modules/ModuleManager.h"

class SMounteaInventoryTemplateEditor;
class SMounteaCraftingRecipesEditor;

class FMounteaAdvancedInventorySystemEditor : public IModuleInterface
{
public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	void RegisterTabSpawners();
	void UnregisterTabSpawners();

	void OpenInventoryTemplateEditor();
	void OpenCraftingRecipesEditor();
	TSharedRef<SDockTab> SpawnCraftingRecipesEditorTab(const FSpawnTabArgs& Args);
	void SetupDefaultsButtonClicked();
	TSharedRef<SDockTab> SpawnInventoryTemplateEditorTab(const FSpawnTabArgs& Args);

	static const FName InventoryTemplateEditorTabId;
	static const FName CraftingRecipesEditorTabId;
	
	void RegisterMenus();

private:

	TSharedPtr<FSlateStyleSet> AdvancedInventorySet;
	TArray<TSharedPtr<class FAssetTypeActions_Base>> AssetActions;
	TArray<FName> RegisteredCustomClassLayouts;
	TArray<FName> RegisteredCustomPropertyTypeLayouts;
	TSharedPtr<class FUICommandList> PluginCommands;
	TWeakPtr<SMounteaInventoryTemplateEditor> CurrentTemplateEditor;
	TWeakPtr<SMounteaCraftingRecipesEditor> CurrentRecipesEditor;
};
