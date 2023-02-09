// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorInventoryPlugin.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FActorInventoryPluginModule"

void FActorInventoryPluginModule::StartupModule()
{
}

void FActorInventoryPluginModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInventoryPluginModule, ActorInventoryPlugin)