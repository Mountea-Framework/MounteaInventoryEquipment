#include "MounteaInventoryEquipmentEditor.h"

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(MounteaInventoryEquipmentEditor);

#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentEditor"

void FMounteaInventoryEquipmentEditor::StartupModule()
{
	UE_LOG(MounteaInventoryEquipmentEditor, Warning, TEXT("MounteaInventoryEquipmentEditor module has been loaded"));
}

void FMounteaInventoryEquipmentEditor::ShutdownModule()
{
	UE_LOG(MounteaInventoryEquipmentEditor, Warning, TEXT("MounteaInventoryEquipmentEditor module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInventoryEquipmentEditor, MounteaInventoryEquipmentEditor)