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

#include "Framework/Commands/Commands.h"
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
