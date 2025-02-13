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

	void LauncherButtonClicked() const;
	void DialoguerButtonClicked() const;
	void YoutubeButtonClicked() const;
	void WikiButtonClicked() const;
	void PluginButtonClicked() const;
	void RegisterMenus();
	void SettingsButtonClicked() const;
	void EditorSettingsButtonClicked() const;
	TSharedRef<SWidget> MakeMounteaMenuWidget() const;

private:

	TSharedPtr<FSlateStyleSet> AdvancedInventorySet;
	TArray<TSharedPtr<class FAssetTypeActions_Base>> AssetActions;

	TSharedPtr<class FUICommandList> PluginCommands;
};