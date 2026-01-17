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

struct FGameplayTagContainer;
struct FGameplayTag;
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
	 * Initializes the Wrapper widget for the Mountea inventory system.
	 * UI Manager (IMounteaAdvancedInventoryUIManagerInterface) knows this wrapper/container
	 * and knows it processes all internal logic.
	 * 
	 * This is the main UI screen/container which should handle all UI related
	 * visual logic, like creating Inventory etc.
	 *
	 * @param Parent The parent UI Manager that implements the IMounteaAdvancedInventoryUIManagerInterface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	void InitializeWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Parent);
	virtual void InitializeWrapperWidget_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Parent) = 0;

	/**
	 * Removes the Wrapper UI from the viewport and cleans up resources.
	 * This will destroy all child UI elements inside the Main Wrapper!
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	void RemoveWrapperWidget();
	virtual void RemoveWrapperWidget_Implementation() = 0;
	
	/**
	 * Returns all currently active Widget State tags tracked by this Wrapper.
	 *
	 * Wrapper widget states are used to keep track of which UI widgets are currently active/visible/in use
	 * within this Wrapper (for example Modal Window, Tooltip, Context Menu, Inventory Panel, etc.).
	 * Each widget that is created/added to the Wrapper should contribute its predefined Gameplay Tag
	 * so the Wrapper and UI Manager can quickly query what is active and react accordingly.
	 *
	 * Typical use cases:
	 * - Query whether a specific UI layer/state is already active (e.g. any modal active).
	 * - Drive input routing rules (e.g. disable inventory input while modal is active).
	 * - UI Manager logic (e.g. prevent stacking conflicting UI screens).
	 *
	 * @return Container of Gameplay Tags representing active UI states/widgets currently tracked by this Wrapper.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	FGameplayTagContainer GetWrapperWidgetStates() const;
	virtual FGameplayTagContainer GetWrapperWidgetStates_Implementation() const = 0;

	/**
	 * Overwrites Wrapper widget state tracking with a new tag container.
	 *
	 * This function replaces the current tracked Widget State tags with the provided container.
	 * It exists primarily as a write-back mechanism for systems that manipulate state tags in bulk
	 * (including helper functions such as AddWidgetStateTag, RemoveWidgetStateTag, ClearWidgetStateTags
	 * or AppendWidgetStateTags).
	 *
	 * Intended usage:
	 * - Synchronize Wrapper state tracking to match real widget lifetime (after rebuild or restore).
	 * - Bulk update the Wrapper when switching UI modes.
	 * - Support C++/Blueprint helper methods which first obtain the container via GetWrapperWidgetStates(),
	 *   modify it, then commit it via SetWrapperWidgetStates().
	 *
	 * @note
	 * - This function only updates state tracking. It should not be assumed to create/destroy widgets by itself.
	 * - Implementations should ensure the stored state remains consistent with actual active widgets.
	 *
	 * @param NewStates Container of Gameplay Tags that will become the Wrapper's active widget states.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	void SetWrapperWidgetStates(const FGameplayTagContainer& NewStates);
	virtual void SetWrapperWidgetStates_Implementation(const FGameplayTagContainer& NewStates) = 0;

	/**
	 * Adds a new Widget State tag to the Wrapper's active state container.
	 *
	 * Wrapper widget states are used to track which UI elements are currently active/visible/in use
	 * within the Wrapper (for example Modal Window, Tooltip, Context Menu, Inventory Panel, etc.).
	 * Each widget added to the Wrapper should contribute its own predefined Gameplay Tag so the Wrapper
	 * and UI Manager can quickly query what is currently running and react accordingly.
	 *
	 * Expected flow:
	 * - A child widget (e.g., ModalWindow) is created/added to the Wrapper.
	 * - That widget provides its state tag definition.
	 * - Wrapper calls AddWidgetStateTag to register that the widget is now active.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to add.
	 * @return True if the tag was added (was not present before). False if the tag was invalid or already present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	bool AddWidgetStateTag(const FGameplayTag& Tag);
	virtual bool AddWidgetStateTag_Implementation(const FGameplayTag& Tag) = 0;

	/**
	 * Removes an existing Widget State tag from the Wrapper's active state container.
	 *
	 * This should be called when a UI element is removed/hidden/destroyed so the Wrapper no longer
	 * reports it as active. This helps prevent stale state and allows the Wrapper/UI Manager to correctly
	 * determine which UI layers are still in use.
	 *
	 * Expected flow:
	 * - A child widget (e.g., ModalWindow) is closed/removed from the Wrapper.
	 * - Wrapper calls RemoveWidgetStateTag to unregister that the widget is no longer active.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to remove.
	 * @return True if the tag was removed. False if the tag was invalid or not found.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	bool RemoveWidgetStateTag(const FGameplayTag& Tag);
	virtual bool RemoveWidgetStateTag_Implementation(const FGameplayTag& Tag) = 0;

	/**
	 * Checks whether the Wrapper currently contains a given Widget State tag.
	 *
	 * This is used to query if a particular UI element/state is currently active within the Wrapper.
	 * For example, the UI Manager or Wrapper may prevent opening another modal if a "UI.Modal.Active"
	 * state already exists, or alter input handling when a context menu state is present.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to check.
	 * @param bExactMatch If true, requires an exact tag match. If false, allows hierarchical matching
	 *                    (e.g., checking "UI.Modal" would match "UI.Modal.Active").
	 * @return True if the Wrapper currently reports the tag as active; otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	bool HasWidgetStateTag(const FGameplayTag& Tag, bool bExactMatch) const;
	virtual bool HasWidgetStateTag_Implementation(const FGameplayTag& Tag, bool bExactMatch) const = 0;

	/**
	 * Clears all Widget State tags tracked by the Wrapper.
	 *
	 * This removes every active widget state tag from the Wrapper at once. It is typically used during
	 * full teardown/reset scenarios such as RemoveWrapperWidget, rebuilding the UI, or when the UI Manager
	 * needs to force the Wrapper into a clean baseline state.
	 *
	 * Note:
	 * - This does not automatically destroy UI widgets by itself. It only clears the tracked state tags.
	 * - Implementations are expected to keep state tracking consistent with actual widget lifetime.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	void ClearWidgetStateTags();
	virtual void ClearWidgetStateTags_Implementation() = 0;

	/**
	 * Appends multiple Widget State tags to the Wrapper's active state container.
	 *
	 * Useful when adding a composite widget (or a UI "mode") that activates multiple tracked states at once,
	 * or when synchronizing the Wrapper to a known set of states provided by another system.
	 *
	 * Expected flow:
	 * - A complex UI element is added (e.g., Inventory Screen + Modal Overlay + Input Blocker),
	 *   providing multiple tags.
	 * - Wrapper calls AppendWidgetStateTags to register all states in one operation.
	 *
	 * @param TagsToAppend Container of Gameplay Tags to add to the Wrapper's active state container.
	 * @return True if at least one new tag was added. False if the container was empty or all tags were already present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Wrapper")
	bool AppendWidgetStateTags(const FGameplayTagContainer& TagsToAppend);
	virtual bool AppendWidgetStateTags_Implementation(const FGameplayTagContainer& TagsToAppend) = 0;
};
