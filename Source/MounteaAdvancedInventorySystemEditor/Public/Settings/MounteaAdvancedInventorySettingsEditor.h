// All rights reserved Dominik Morse 2024

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
