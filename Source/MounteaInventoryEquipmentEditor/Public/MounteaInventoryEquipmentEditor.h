#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(MounteaInventoryEquipmentEditor, All, All);

class FMounteaInventoryEquipmentEditor : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	TSharedPtr<class FMounteaInventoryItemConfigAssetAction> MounteaInventoryItemConfigAssetAction;
};