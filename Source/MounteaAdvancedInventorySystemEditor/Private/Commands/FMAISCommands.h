// All rights reserved Dominik Morse 2024

#pragma once
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"

class FMAISCommands : public TCommands<FMAISCommands>
{
public:

	FMAISCommands()
	: TCommands<FMAISCommands>(
		TEXT("MounteaSupport"),
		NSLOCTEXT("Contexts", "Support", "Mountea Advanced Inventory System"),
		NAME_None,
		FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	
	TSharedPtr< FUICommandInfo > PluginAction;
	
};
