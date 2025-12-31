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

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Definitions/MounteaAdvancedInventoryEditorTypes.h"
#include "MounteaAdvancedInventorySettingsEditor.generated.h"

/**
 * 
 */
UCLASS(config = MounteaSettings, defaultconfig, meta = (DisplayName = "Mountea Inventory System Settings"))
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventorySettingsEditor : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsEditor();
	
	// If True, Icons will display Text next to icons.
	UPROPERTY(Config, EditAnywhere, Category="Config")
	uint8 bDisplayEditorButtonText : 1;
	
	// Shared styling for all documentation pages.
	UPROPERTY(Config, EditAnywhere, Category = "Editor Templates",
		AdvancedDisplay, meta = (FilePathFilter = "css"))
	FFilePath SharedStylesheetPath;
	
	// Shared script for all documentation pages.
	UPROPERTY(Config, EditAnywhere, Category = "Editor Templates",
		AdvancedDisplay, meta = (FilePathFilter = "js"))
	FFilePath SharedScriptPath;

	/**
	 * Defines the list of tutorial pages.
	 * You can easily extend those if needed for your colleagues.
	 */
	UPROPERTY(Config, EditAnywhere, Category="Editor Templates",
		AdvancedDisplay)
	TMap<int32, FItemTemplateEditorPageConfig> EditorTemplatePages;

public:

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaAdvancedInventorySystemEditor", "MounteaSettingsEditorDescription", "Mountea Inventory System (Editor)");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaAdvancedInventorySystemEditor", "MounteaSettingsEditorDescription", "Default values for Editor Mountea Advanced Inventory Plugin.");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}
#endif
};
