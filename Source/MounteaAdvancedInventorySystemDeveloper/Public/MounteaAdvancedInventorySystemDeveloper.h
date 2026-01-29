// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

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