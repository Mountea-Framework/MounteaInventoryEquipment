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
#include "MounteaAdvancedInventoryItemActionWidgetInterface.generated.h"

class UMounteaInventoryItemAction;
class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryItemActionWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryItemActionWidgetInterface defines individual item action widget functionality.
 * Item action interfaces manage individual actionable buttons and controls for inventory item operations
 * such as use, equip, drop, and other context-specific item interactions.
 *
 * @see [Item Actions](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryItemActionsContainerWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryItemActionWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	void InitializeItemActions(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, UMounteaInventoryItemAction* ItemAction, const FGuid& ItemId);
	virtual void InitializeItemActions_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI, UMounteaInventoryItemAction* ItemAction, const FGuid& ItemId) = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	bool IsActionEnabled() const;
	virtual bool IsActionEnabled_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	bool IsActionValid() const;
	virtual bool IsActionValid_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemActions")
	void ExecuteItemAction();
	virtual void ExecuteItemAction_Implementation() = 0;
};
