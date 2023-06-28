// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "MIEHelpStyle.h"


class FMIECommands : public TCommands<FMIECommands>
{
public:

	FMIECommands()
	: TCommands<FMIECommands>(TEXT("MounteaSupport"), NSLOCTEXT("Contexts", "Support", "Mountea Dialogue System"), NAME_None, FMIEHelpStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	
	TSharedPtr< FUICommandInfo > PluginAction;
	//TSharedPtr< FUICommandInfo > WikiAction;
};
