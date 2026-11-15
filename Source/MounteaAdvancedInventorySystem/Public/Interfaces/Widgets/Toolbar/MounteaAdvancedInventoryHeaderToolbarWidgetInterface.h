// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryHeaderToolbarWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryHeaderToolbarWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * MounteaAdvancedInventoryHeaderToolbarWidgetInterface
 * Top bar widget: screen title, screen tabs and player status. Empty marker for now.
 *
 * @see [User Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryHeaderToolbarWidgetInterface
{
	GENERATED_BODY()

public:

};
