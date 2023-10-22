#include "MounteaInventoryEquipmentDeveloper.h"

DEFINE_LOG_CATEGORY(MounteaInventoryEquipmentDeveloper);

#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentDeveloper"

void FMounteaInventoryEquipmentDeveloper::StartupModule()
{
	UE_LOG(MounteaInventoryEquipmentDeveloper, Warning, TEXT("MounteaInventoryEquipmentDeveloper module has been loaded"));
}

void FMounteaInventoryEquipmentDeveloper::ShutdownModule()
{
	UE_LOG(MounteaInventoryEquipmentDeveloper, Warning, TEXT("MounteaInventoryEquipmentDeveloper module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInventoryEquipmentDeveloper, MounteaInventoryEquipmentDeveloper)