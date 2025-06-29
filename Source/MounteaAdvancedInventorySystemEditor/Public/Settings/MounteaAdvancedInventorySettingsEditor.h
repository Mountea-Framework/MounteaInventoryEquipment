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
