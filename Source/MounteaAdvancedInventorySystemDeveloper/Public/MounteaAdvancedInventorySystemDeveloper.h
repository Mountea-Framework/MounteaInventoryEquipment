#pragma once

#include "Modules/ModuleManager.h"

class FMounteaInventoryPinFactory;

DECLARE_LOG_CATEGORY_EXTERN(MounteaAdvancedInventorySystemDeveloper, All, All);

class FMounteaAdvancedInventorySystemDeveloper : public IModuleInterface
{
public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;
	
#if WITH_EDITOR
private:
	
	TSharedPtr<FMounteaInventoryPinFactory> PinFactory;
#endif
	
};