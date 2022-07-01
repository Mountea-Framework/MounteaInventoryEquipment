#include "InventoryEditorNotifications.h"

DEFINE_LOG_CATEGORY(InventoryEditorNotifications);

#define LOCTEXT_NAMESPACE "FInventoryEditorNotifications"

void FInventoryEditorNotifications::StartupModule()
{
	UE_LOG(InventoryEditorNotifications, Warning, TEXT("InventoryEditorNotifications module has been loaded"));
}

void FInventoryEditorNotifications::ShutdownModule()
{
	UE_LOG(InventoryEditorNotifications, Warning, TEXT("InventoryEditorNotifications module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInventoryEditorNotifications, InventoryEditorNotifications)