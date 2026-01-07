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

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventorySystemWrapperWidgetInterface.generated.h"

class IMounteaAdvancedInventoryInterface;
class IMounteaAdvancedInventoryUIManagerInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventorySystemWrapperWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaInventorySystemWrapperWidgetInterface manages main inventory system wrapper widget.
 * System base interfaces handle initialization, visibility management, and inventory source assignment
 * for top-level inventory UI wrappers and container widgets.
 *
 * @see [System Base Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryUIManagerInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventorySystemWrapperWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param Parent 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void InitializeWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Parent);
	virtual void InitializeWrapperWidget_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Parent) = 0;

	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void RemoveWrapperWidget();
	virtual void RemoveWrapperWidget_Implementation() = 0;
	
	// TODO: Apply sorting
	// TODO: Apply filtering
};
