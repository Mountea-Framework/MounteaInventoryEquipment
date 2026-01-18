#include "MounteaAdvancedInventorySystemDeveloper.h"

#if WITH_EDITOR
#include "Factories/MounteaAdvancedInventoryPinFactory.h"
#include "EdGraphUtilities.h"
#endif

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
	
#if WITH_EDITOR
	// Register Pin Factory
	{
		PinFactory = MakeShared<FMounteaInventoryPinFactory>();
		FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
	}
#endif
	
}

void FMounteaAdvancedInventorySystemDeveloper::ShutdownModule()
{
	// Style Shutdown
	{
		FMounteaAdvancedInventoryDeveloperStyle::Shutdown();
	}
	
#if WITH_EDITOR
	// Unregister Pin Factory
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
		PinFactory.Reset();
	}
#endif
	
	UE_LOG(MounteaAdvancedInventorySystemDeveloper, Warning, TEXT("MounteaAdvancedInventorySystemDeveloper module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaAdvancedInventorySystemDeveloper, MounteaAdvancedInventorySystemDeveloper)