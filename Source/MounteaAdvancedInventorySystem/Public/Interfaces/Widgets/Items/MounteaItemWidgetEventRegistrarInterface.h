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
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemWidgetInterface.h"
#include "UObject/Interface.h"
#include "MounteaItemWidgetEventRegistrarInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaItemWidgetEventRegistrarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaItemWidgetEventRegistrarInterface lets a native manager (e.g. UMounteaInventoryUIComponent,
 * UMounteaCraftingParticipantUIComponent) host the Bind/Unbind/Trigger registry for individual Item
 * widget "selected" events, without the Item widget itself ever needing to know which gameplay
 * system owns it.
 *
 * Intended to be implemented only by native UObjects capable of storing a per-widget delegate registry
 * (in practice, native UActorComponents). Item widgets never Cast<> to this interface - they only
 * hold an opaque UObject reference (see IMounteaAdvancedInventoryItemWidgetInterface) and reach it
 * through UMounteaItemWidgetStatics, which always resolves via Execute_/Implements<>().
 *
 * @see [Item Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryItemWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaItemWidgetEventRegistrarInterface
{
	GENERATED_BODY()

public:

	/**
	 * Binds an external listener to a specific Item widget instance's OnItemWidgetSelected event.
	 *
	 * @param ItemWidget The Item widget instance to listen to.
	 * @param Binding The delegate to invoke once the widget fires its event.
	 * @return True if the binding was registered, false otherwise (invalid widget/unbound delegate).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Item")
	bool BindItemWidgetSelected(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding);
	virtual bool BindItemWidgetSelected_Implementation(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding) = 0;

	/**
	 * Removes a previously registered binding for a specific Item widget instance.
	 *
	 * @param ItemWidget The Item widget instance the binding was registered against.
	 * @param Binding The delegate to remove.
	 * @return True if a binding was removed, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Item")
	bool UnbindItemWidgetSelected(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding);
	virtual bool UnbindItemWidgetSelected_Implementation(UObject* ItemWidget, const FMounteaItemWidgetSelectedBinding& Binding) = 0;

	/**
	 * Fires the OnItemWidgetSelected event for a specific Item widget instance, notifying any bound listeners.
	 *
	 * @param ItemWidget The Item widget instance that was selected.
	 * @param ItemId The item identifier carried by the event.
	 * @return True if the event was delivered to at least one listener, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Item")
	bool TriggerItemWidgetSelected(UObject* ItemWidget, const FGuid& ItemId);
	virtual bool TriggerItemWidgetSelected_Implementation(UObject* ItemWidget, const FGuid& ItemId) = 0;
};
