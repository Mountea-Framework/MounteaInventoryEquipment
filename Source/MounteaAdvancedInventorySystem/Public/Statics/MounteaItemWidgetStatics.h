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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaItemWidgetStatics.generated.h"

/**
 * Shared Blueprint-callable statics for the Item widget OnItemWidgetSelected event, usable by
 * any gameplay system (Inventory, Crafting, or future systems) without WBP_Item/WBP_Items
 * ever needing system-specific knowledge. Bind/Unbind/Trigger resolve the widget's stored registrar
 * (see IMounteaAdvancedInventoryItemWidgetInterface::GetItemWidgetEventRegistrar) internally,
 * so callers only ever need to pass the Item widget itself.
 *
 * @see [Item Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryItemWidgetInterface
 * @see IMounteaItemWidgetEventRegistrarInterface
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaItemWidgetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Sets the registrar object an Item widget will route its OnItemWidgetSelected event through.
	 * Should be called once, right after the widget is created, by whoever spawns it.
	 *
	 * @param ItemWidget Widget implementing IMounteaAdvancedInventoryItemWidgetInterface.
	 * @param Registrar Object implementing IMounteaItemWidgetEventRegistrarInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Item",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="ItemWidget"),
		meta=(DisplayName="Set Item Widget Event Registrar"))
	static void SetItemWidgetEventRegistrar(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface")) UObject* ItemWidget,
		UObject* Registrar);

	/**
	 * Returns the registrar previously set via SetItemWidgetEventRegistrar.
	 *
	 * @param ItemWidget Widget implementing IMounteaAdvancedInventoryItemWidgetInterface.
	 * @return Object implementing IMounteaItemWidgetEventRegistrarInterface, or nullptr if unset.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Item",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="ItemWidget"),
		meta=(DisplayName="Get Item Widget Event Registrar"))
	static UObject* GetItemWidgetEventRegistrar(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface")) UObject* ItemWidget);

	/**
	 * Binds a listener to a specific Item widget instance's OnItemWidgetSelected event.
	 * Resolves the widget's stored registrar internally, so no registrar needs to be passed here.
	 *
	 * @param ItemWidget Widget implementing IMounteaAdvancedInventoryItemWidgetInterface.
	 * @param Binding The delegate to invoke once the widget fires its event.
	 * @return True if the binding was registered, false otherwise (invalid widget, unbound delegate, or no registrar set).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Item|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="ItemWidget"),
		meta=(DisplayName="Bind On Item Widget Selected"))
	static bool BindToOnItemWidgetSelected(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface")) UObject* ItemWidget,
		const FMounteaItemWidgetSelectedBinding& Binding);

	/**
	 * Removes a previously registered binding from a specific Item widget instance's OnItemWidgetSelected event.
	 *
	 * @param ItemWidget Widget implementing IMounteaAdvancedInventoryItemWidgetInterface.
	 * @param Binding The delegate to remove.
	 * @return True if a binding was removed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Item|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="ItemWidget"),
		meta=(DisplayName="Unbind From On Item Widget Selected"))
	static bool UnbindFromOnItemWidgetSelected(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface")) UObject* ItemWidget,
		const FMounteaItemWidgetSelectedBinding& Binding);

	/**
	 * Fires the OnItemWidgetSelected event for an Item widget, notifying any bound listeners.
	 * Resolves the widget's stored registrar and item id internally - call this from the
	 * widget's own click handler with no other parameters.
	 *
	 * @param ItemWidget Widget implementing IMounteaAdvancedInventoryItemWidgetInterface.
	 * @return True if the event was delivered to at least one listener, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Item",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="ItemWidget"),
		meta=(DisplayName="Trigger On Item Widget Selected"))
	static bool TriggerOnItemWidgetSelected(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface")) UObject* ItemWidget);

	/**
	 * Requests this widget to select this inventory item in the UI.
	 * Moved here from UMounteaInventoryUIStatics::ItemWidget_SelectItem to keep this event/select
	 * group self-contained.
	 *
	 * @param ItemWidget Widget implementing IMounteaAdvancedInventoryItemWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Item",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="ItemWidget"),
		meta=(DisplayName="Select Item"))
	static void SelectItem(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface")) UObject* ItemWidget);

private:

	static UObject* ResolveRegistrar(const UObject* ItemWidget);
};
