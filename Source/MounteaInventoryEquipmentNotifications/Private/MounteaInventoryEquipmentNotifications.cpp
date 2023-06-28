#include "MounteaInventoryEquipmentNotifications.h"

DEFINE_LOG_CATEGORY(MounteaInventoryEquipmentNotifications);

#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentNotifications"

void FMounteaInventoryEquipmentNotifications::StartupModule()
{
	UE_LOG(MounteaInventoryEquipmentNotifications, Warning, TEXT("MounteaInventoryEquipmentNotifications module has been loaded"));
}

void FMounteaInventoryEquipmentNotifications::ShutdownModule()
{
	UE_LOG(MounteaInventoryEquipmentNotifications, Warning, TEXT("MounteaInventoryEquipmentNotifications module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInventoryEquipmentNotifications, MounteaInventoryEquipmentNotifications)