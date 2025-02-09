#include "MounteaAdvancedInventorySystemDeveloper.h"

#include "Styling/MounteaAdvancedInventoryDeveloperStyle.h"

DEFINE_LOG_CATEGORY(MounteaAdvancedInventorySystemDeveloper);

#define LOCTEXT_NAMESPACE "FMounteaAdvancedInventorySystemDeveloper"

void FMounteaAdvancedInventorySystemDeveloper::StartupModule()
{
	UE_LOG(MounteaAdvancedInventorySystemDeveloper, Warning, TEXT("MounteaAdvancedInventorySystemDeveloper module has been loaded"));

	// Register Style
	{
		FMounteaAdvancedInventoryDeveloperStyle::Initialize();
	}
}

void FMounteaAdvancedInventorySystemDeveloper::ShutdownModule()
{
	// Style Shutdown
	{
		FMounteaAdvancedInventoryDeveloperStyle::Shutdown();
	}
	
	UE_LOG(MounteaAdvancedInventorySystemDeveloper, Warning, TEXT("MounteaAdvancedInventorySystemDeveloper module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaAdvancedInventorySystemDeveloper, MounteaAdvancedInventorySystemDeveloper)