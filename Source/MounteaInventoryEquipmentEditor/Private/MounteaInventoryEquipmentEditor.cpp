#include "MounteaInventoryEquipmentEditor.h"

#include "AssetToolsModule.h"
#include "AssetActions/FMounteaInventoryItemConfigAssetAction.h"
#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(MounteaInventoryEquipmentEditor);

#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentEditor"

void FMounteaInventoryEquipmentEditor::StartupModule()
{
	// Asset Actions
	{
		MounteaInventoryItemConfigAssetAction = MakeShared<FMounteaInventoryItemConfigAssetAction>();
		FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(MounteaInventoryItemConfigAssetAction.ToSharedRef());
	}
	
	UE_LOG(MounteaInventoryEquipmentEditor, Warning, TEXT("MounteaInventoryEquipmentEditor module has been loaded"));
}

void FMounteaInventoryEquipmentEditor::ShutdownModule()
{
	UE_LOG(MounteaInventoryEquipmentEditor, Warning, TEXT("MounteaInventoryEquipmentEditor module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInventoryEquipmentEditor, MounteaInventoryEquipmentEditor)