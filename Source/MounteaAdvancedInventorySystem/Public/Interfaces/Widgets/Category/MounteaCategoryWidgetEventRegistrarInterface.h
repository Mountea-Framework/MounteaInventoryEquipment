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
#include "Interfaces/Widgets/Category/MounteaAdvancedInventoryCategoryWidgetInterface.h"
#include "UObject/Interface.h"
#include "MounteaCategoryWidgetEventRegistrarInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaCategoryWidgetEventRegistrarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaCategoryWidgetEventRegistrarInterface lets a native manager (e.g. UMounteaInventoryUIComponent,
 * UMounteaCraftingParticipantUIComponent) host the Bind/Unbind/Trigger registry for individual Category
 * widget "selected" events, without the Category widget itself ever needing to know which gameplay
 * system owns it.
 *
 * Intended to be implemented only by native UObjects capable of storing a per-widget delegate registry
 * (in practice, native UActorComponents). Category widgets never Cast<> to this interface - they only
 * hold an opaque UObject reference (see IMounteaAdvancedInventoryCategoryWidgetInterface) and reach it
 * through UMounteaCategoryWidgetStatics, which always resolves via Execute_/Implements<>().
 *
 * @see [Category Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryCategoryWidgetInterface
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaCategoryWidgetEventRegistrarInterface
{
	GENERATED_BODY()

public:

	/**
	 * Binds an external listener to a specific Category widget instance's OnCategorySelected event.
	 *
	 * @param CategoryWidget The Category widget instance to listen to.
	 * @param Binding The delegate to invoke once the widget fires its event.
	 * @return True if the binding was registered, false otherwise (invalid widget/unbound delegate).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Category")
	bool BindCategoryWidgetSelected(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding);
	virtual bool BindCategoryWidgetSelected_Implementation(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding) = 0;

	/**
	 * Removes a previously registered binding for a specific Category widget instance.
	 *
	 * @param CategoryWidget The Category widget instance the binding was registered against.
	 * @param Binding The delegate to remove.
	 * @return True if a binding was removed, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Category")
	bool UnbindCategoryWidgetSelected(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding);
	virtual bool UnbindCategoryWidgetSelected_Implementation(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding) = 0;

	/**
	 * Fires the OnCategorySelected event for a specific Category widget instance, notifying any bound listeners.
	 *
	 * @param CategoryWidget The Category widget instance that was selected.
	 * @param CategoryId The category identifier carried by the event.
	 * @return True if the event was delivered to at least one listener, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Category")
	bool TriggerCategoryWidgetSelected(UObject* CategoryWidget, const FString& CategoryId);
	virtual bool TriggerCategoryWidgetSelected_Implementation(UObject* CategoryWidget, const FString& CategoryId) = 0;
};
