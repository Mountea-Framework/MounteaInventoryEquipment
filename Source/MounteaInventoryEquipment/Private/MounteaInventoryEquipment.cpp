// Copyright Epic Games, Inc. All Rights Reserved.

#include "MounteaInventoryEquipment.h"

#include "GameplayTagsManager.h"
#include "Interfaces/IPluginManager.h"


#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentModule"

void FMounteaInventoryEquipmentModule::StartupModule()
{
	TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"));
	check(ThisPlugin.IsValid());
	
	UGameplayTagsManager::Get().AddTagIniSearchPath(ThisPlugin->GetBaseDir() / TEXT("Config") / TEXT("Tags"));
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMounteaInventoryEquipmentModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMounteaInventoryEquipmentModule, MounteaInventoryEquipment)