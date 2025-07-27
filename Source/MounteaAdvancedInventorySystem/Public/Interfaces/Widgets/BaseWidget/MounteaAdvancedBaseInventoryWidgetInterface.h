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
#include "MounteaAdvancedBaseInventoryWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedBaseInventoryWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedBaseInventoryWidgetInterface establishes parent-child relationships for inventory widgets.
 * Base inventory widget interfaces manage ownership associations between widgets and their parent
 * inventory UI systems for proper widget hierarchy and communication.
 *
 * @see [Base Inventory Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryUIInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedBaseInventoryWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Sets the reference to the owning inventory UI interface.
	 * This function allows the widget to be associated with a specific inventory UI.
	 *
	 * @param NewOwningInventoryUI A TScriptInterface reference to the inventory UI interface that owns this widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Base")
	void SetOwningInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI);
	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI) = 0;

	/**
	 * Retrieves the owning inventory UI interface for this widget.
	 * This function allows other components to access the parent inventory UI.
	 *
	 * @return The TScriptInterface reference to the owning inventory UI interface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Base")
	TScriptInterface<IMounteaAdvancedInventoryUIInterface> GetOwningInventoryUI() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryUIInterface> GetOwningInventoryUI_Implementation() const = 0;
};
