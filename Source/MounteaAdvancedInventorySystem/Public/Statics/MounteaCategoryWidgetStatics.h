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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaCategoryWidgetStatics.generated.h"

/**
 * Shared Blueprint-callable statics for the Category widget OnCategorySelected event, usable by
 * any gameplay system (Inventory, Crafting, or future systems) without WBP_Category/WBP_Categories
 * ever needing system-specific knowledge. Bind/Unbind/Trigger resolve the widget's stored registrar
 * (see IMounteaAdvancedInventoryCategoryWidgetInterface::GetCategoryWidgetEventRegistrar) internally,
 * so callers only ever need to pass the Category widget itself.
 *
 * @see [Category Widgets](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see IMounteaAdvancedInventoryCategoryWidgetInterface
 * @see IMounteaCategoryWidgetEventRegistrarInterface
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCategoryWidgetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Sets the registrar object a Category widget will route its OnCategorySelected event through.
	 * Should be called once, right after the widget is created, by whoever spawns it.
	 *
	 * @param CategoryWidget Widget implementing IMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @param Registrar Object implementing IMounteaCategoryWidgetEventRegistrarInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Category",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="CategoryWidget"),
		meta=(DisplayName="Set Category Widget Event Registrar"))
	static void SetCategoryWidgetEventRegistrar(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface")) UObject* CategoryWidget,
		UObject* Registrar);

	/**
	 * Returns the registrar previously set via SetCategoryWidgetEventRegistrar.
	 *
	 * @param CategoryWidget Widget implementing IMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @return Object implementing IMounteaCategoryWidgetEventRegistrarInterface, or nullptr if unset.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Category",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="CategoryWidget"),
		meta=(DisplayName="Get Category Widget Event Registrar"))
	static UObject* GetCategoryWidgetEventRegistrar(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface")) UObject* CategoryWidget);

	/**
	 * Binds a listener to a specific Category widget instance's OnCategorySelected event.
	 * Resolves the widget's stored registrar internally, so no registrar needs to be passed here.
	 *
	 * @param CategoryWidget Widget implementing IMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @param Binding The delegate to invoke once the widget fires its event.
	 * @return True if the binding was registered, false otherwise (invalid widget, unbound delegate, or no registrar set).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Category|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="CategoryWidget"),
		meta=(DisplayName="Bind On Category Widget Selected"))
	static bool BindToOnCategoryWidgetSelected(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface")) UObject* CategoryWidget,
		const FMounteaCategoryWidgetSelectedBinding& Binding);

	/**
	 * Removes a previously registered binding from a specific Category widget instance's OnCategorySelected event.
	 *
	 * @param CategoryWidget Widget implementing IMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @param Binding The delegate to remove.
	 * @return True if a binding was removed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Category|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="CategoryWidget"),
		meta=(DisplayName="Unbind From On Category Widget Selected"))
	static bool UnbindFromOnCategoryWidgetSelected(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface")) UObject* CategoryWidget,
		const FMounteaCategoryWidgetSelectedBinding& Binding);

	/**
	 * Fires the OnCategorySelected event for a Category widget, notifying any bound listeners.
	 * Resolves the widget's stored registrar and category id internally - call this from the
	 * widget's own click handler with no other parameters.
	 *
	 * @param CategoryWidget Widget implementing IMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @return True if the event was delivered to at least one listener, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Category",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="CategoryWidget"),
		meta=(DisplayName="Trigger On Category Widget Selected"))
	static bool TriggerOnCategoryWidgetSelected(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface")) UObject* CategoryWidget);
	
	/**
	 * Requests this widget to select its assigned inventory category.
	 *
	 * @param CategoryWidget  Widget implementing IMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @return True if the category selection was accepted or routed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Category",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="CategoryWidget"),
		meta=(DisplayName="Select Category"))
	static bool SelectCategory(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface")) UObject* CategoryWidget);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Category",
		meta=(MounteaGetter),
		meta=(DisplayName="Calculate New Active Category"))
	static FString CalculateNewSelectedCategory(const int32 Offset, const TArray<FString>& Categories, const FString& ActiveCategory, const bool Overscroll = false);

private:

	static UObject* ResolveRegistrar(const UObject* CategoryWidget);
};
