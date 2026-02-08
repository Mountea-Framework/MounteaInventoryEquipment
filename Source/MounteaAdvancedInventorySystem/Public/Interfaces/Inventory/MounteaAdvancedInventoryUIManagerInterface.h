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
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryUIManagerInterface.generated.h"

struct FInventoryItem;
struct FInventoryNotificationData;

class IMounteaAdvancedInventoryInterface;

class UMounteaSelectableInventoryItemAction;
class UWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryCategorySelected, const FString&, SelectedCategoryId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemSelected, const FGuid&, SelectedItemId);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryUIManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryUIManagerInterface manages user interface operations for inventory systems.
 * UI interfaces handle widget creation, visibility management, category selection, item interaction,
 * notification display, and grid slot management for comprehensive inventory interface control.
 *
 * @see [Inventory UI Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see UMounteaInventoryUIComponent
 * @see FMounteaInventoryGridSlot
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryUIManagerInterface
{
	GENERATED_BODY()

public:
	
	// --- Inventory

	/**
	 * Retrieves the parent inventory associated with this UI.
	 * @return The parent inventory interface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const = 0;

	/**
	 * Sets the parent inventory for this UI.
	 * @param NewParentInventory - The inventory interface to associate.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) = 0;
	
	/**
	 * Creates the inventory UI.
	 * @return True if UI was successfully created, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	bool CreateWrapperWidget();
	virtual bool CreateWrapperWidget_Implementation() = 0;

	/**
	 * Returns the inventory UI.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	UUserWidget* GetWrapperWidget() const;
	virtual UUserWidget* GetWrapperWidget_Implementation() const = 0;

	/**
	 * Removes the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Manager")
	void RemoveWrapperWidget();
	virtual void RemoveWrapperWidget_Implementation() = 0;
	
	/**
	 * Creates the inventory UI.
	 * @return True if UI was successfully created, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory")
	bool CreateInventoryWidget();
	virtual bool CreateInventoryWidget_Implementation() = 0;

	/**
	 * Returns the inventory UI.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory")
	UUserWidget* GetInventoryWidget() const;
	virtual UUserWidget* GetInventoryWidget_Implementation() const = 0;
	
	/**
	 * Sets the inventory UI from outside.
	 * @return True if UI was successfully set, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory")
	bool SetInventoryWidget(UUserWidget* NewInventoryWidget);
	virtual bool SetInventoryWidget_Implementation(UUserWidget* NewInventoryWidget) = 0;

	/**
	 * Removes the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory")
	void RemoveInventoryWidget();
	virtual void RemoveInventoryWidget_Implementation() = 0;
	
	// --- Notification

	/**
	 * Retrieves the notification container widget.
	 * @return A pointer to the notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	UUserWidget* GetNotificationContainer() const;
	virtual UUserWidget* GetNotificationContainer_Implementation() const = 0;

	/**
	 * Sets the notification container widget.
	 * @param NewNotificationContainer - The new notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	void SetNotificationContainer(UUserWidget* NewNotificationContainer);
	virtual void SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer) = 0;

	/**
	 * Creates a new inventory notification.
	 * @param NotificationData - The data defining the notification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	/**
	 * Removes all active inventory notifications.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Notifications")
	void RemoveInventoryNotifications();
	virtual void RemoveInventoryNotifications_Implementation() = 0;

	// --- Categories
	
	/**
	 * Handles the selection of a category in the inventory UI.
	 * @param SelectedCategoryId The ID of the selected category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Category")
	void CategorySelected(const FString& SelectedCategoryId);
	virtual void CategorySelected_Implementation(const FString& SelectedCategoryId) = 0;

	/**
	 * Retrieves the currently selected category ID.
	 * @return The ID of the currently selected category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Category")
	FString GetSelectedCategoryId() const;
	virtual FString GetSelectedCategoryId_Implementation() const = 0;
	
	// --- Item

	/**
	 * Retrieves the currently active item widget in the inventory UI.
	 *
	 * This function returns a pointer to the active item widget currently being displayed or interacted with.
	 *
	 * @return A pointer to the UWidget representing the active item widget, or nullptr if no widget is active.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Item")
	UWidget* GetActiveItemWidget() const;
	virtual UWidget* GetActiveItemWidget_Implementation() const = 0;

	/**
	 * Sets the active item widget in the UI.
	 *
	 * @param NewActiveItemWidget The new widget to be set as the active item widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Item")
	void SetActiveItemWidget(UWidget* NewActiveItemWidget);
	virtual void SetActiveItemWidget_Implementation(UWidget* NewActiveItemWidget) = 0;

	/**
	 * Processes an item that has been added to the inventory.
	 * @param AddedItem The item that has been added to the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Item")
	void ProcessItemAdded(const FInventoryItem& AddedItem);
	virtual void ProcessItemAdded_Implementation(const FInventoryItem& AddedItem) = 0;

	/**
	 * Processes an item that has been removed from the inventory.
	 * @param RemovedItem The item that has been removed from the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Item")
	void ProcessItemRemoved(const FInventoryItem& RemovedItem);
	virtual void ProcessItemRemoved_Implementation(const FInventoryItem& RemovedItem) = 0;

	/**
	 * Processes an item that has been modified in the inventory.
	 * @param ModifiedItem The item that has been modified in the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Item")
	void ProcessItemModified(const FInventoryItem& ModifiedItem);
	virtual void ProcessItemModified_Implementation(const FInventoryItem& ModifiedItem) = 0;
	
	/**
	 * Handles the selection of an item in the inventory UI.
	 * @param SelectedItem The GUID of the selected item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Item")
	void ItemSelected(const FGuid& SelectedItem);
	virtual void ItemSelected_Implementation(const FGuid& SelectedItem) = 0;

	/**
	 * Retrieves the currently active item GUID.
	 * @return The GUID of the currently active item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Item")
	FGuid GetActiveItemGuid() const;
	virtual FGuid GetActiveItemGuid_Implementation() const = 0;
	
	// --- Items

	/**
	 * Retrieves local custom items map.
	 * 
	 * @return Custom Items Mpa if any specified.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Items")
	TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap() const;
	virtual TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap_Implementation() const = 0;
	
	/**
	 * Adds (or replaces) a single entry in the custom items map.
	 *
	 * @param ItemTag   Gameplay tag representing the custom item key.
	 * @param ItemId    Guid value associated with the item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Items")
	void AddCustomItemToMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual void AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Appends entries from another map into the custom items map.
	 * Existing keys will be overwritten with new values.
	 *
	 * @param OtherItems   Map of custom items to merge in.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Items")
	void AppendCustomItemsMap(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems);
	virtual void AppendCustomItemsMap_Implementation(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems) = 0;

	/**
	 * Clears all entries from the custom items map.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Items")
	void ClearCustomItemsMap();
	virtual void ClearCustomItemsMap_Implementation() = 0;

	/**
	 * Removes a single entry from the custom items map.
	 *
	 * @param ItemTag Key to remove the item from.
	 * @param ItemId Guid representing the custom item key to remove.
	 *
	 * @return True if an entry was removed, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Items")
	bool RemoveCustomItemFromMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual bool RemoveCustomItemFromMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Validates whether the provided Item guid is stored in a map for specified tag.
	 * Example:
	 * - Is Item in Favorites
	 * 
	 * @param ItemTag Tag which defines the key, like "Favorite"
	 * @param ItemId Item guid to search for
	 * @return True if item is stored in such container, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Inventory|Manager|Items")
	bool IsItemStoredInCustomMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual bool IsItemStoredInCustomMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;
	
	// --- General
	
	/**
	 * This function allows UI Manager to listen to external systems which might want to process Widget Commands.
	 * Example might to a request to refresh item count when Item Action consumes it.
	 * 
	 * Chain of hierarchy is respected, so commands are given to root and then based on the selection passed down.
	 * 
	 * @param Command Command to Process
	 * @param OptionalPayload Optional Payload to pass with the command
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory")
	void ExecuteWidgetCommand(const FString& Command, UObject* OptionalPayload);
	virtual void ExecuteWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload) = 0;
	
	// --- Item Actions

	/**
	 * Returns a snapshot of Selectable Item Actions currently waiting in the UI Manager queue.
	 *
	 * The returned array represents the current pending actions stored by the UI Manager. The queue does not
	 * execute actions automatically; actions are expected to drive their own completion/cancellation flow and
	 * notify the UI Manager when they should be removed.
	 *
	 * @return Array of queued Selectable Item Actions.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	TArray<UMounteaSelectableInventoryItemAction*> GetItemActionsQueue() const;
	virtual TArray<UMounteaSelectableInventoryItemAction*> GetItemActionsQueue_Implementation() const = 0;

	/**
	 * Registers (enqueues) a Selectable Item Action together with an optional payload object.
	 *
	 * This queue acts as a lightweight registry of pending UI-gated actions. Enqueuing does not start execution,
	 * tick, or advance any processing. The action itself is responsible for initiating UI interaction, waiting for
	 * approval/decline, and then calling CompleteQueuedAction or CancelQueuedAction to remove itself from the queue.
	 *
	 * @param ItemAction Selectable Item Action to register.
	 * @param Payload Optional data container/context associated with this queued action instance.
	 *
	 * @return True if the action was successfully enqueued, false otherwise (invalid action).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	bool EnqueueItemAction(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload);
	virtual bool EnqueueItemAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload) = 0;

	/**
	 * Removes all queued Selectable Item Actions from the UI Manager queue.
	 *
	 * This function clears the queue registry only. It does not attempt to complete or cancel individual actions,
	 * nor does it notify them. Any UI or async work owned by the actions remains their responsibility.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	void EmptyItemActionsQueue();
	virtual void EmptyItemActionsQueue_Implementation() = 0;

	/**
	 * Marks the specified queued Selectable Item Action as completed and removes it from the queue.
	 *
	 * This function is typically called by the action itself after its UI-gated flow has been approved and the action
	 * has executed its final logic (e.g. consume amount selected in a modal). The UI Manager uses this call to stop
	 * tracking the action in its pending registry.
	 *
	 * @param ItemAction Selectable Item Action to remove from the queue.
	 * @param Payload Optional payload received for completion of the Action. Usually contains "returned" values (eg. how much quantity was requested to consume).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	void CompleteQueuedAction(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload);
	virtual void CompleteQueuedAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload) = 0;

	/**
	 * Cancels the specified queued Selectable Item Action and removes it from the queue.
	 *
	 * This function is typically called by the action itself when the UI-gated flow is declined/cancelled
	 * (e.g. modal closed, user pressed cancel). The UI Manager uses this call to stop tracking the action
	 * in its pending registry.
	 *
	 * @param ItemAction Selectable Item Action to remove from the queue.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	void CancelQueuedAction(UMounteaSelectableInventoryItemAction* ItemAction);
	virtual void CancelQueuedAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction) = 0;
	
	// --- Widget States
	
	/**
	 * Returns all currently active Widget State tags tracked by the target UI Manager.
	 *
	 * UI Manager states are used to keep track of which UI widgets are currently active/visible/in use
	 * within the Wrapper (for example Modal Window, Tooltip, Context Menu, Inventory Panel, etc.).
	 * Each widget that is created/added to the Wrapper should contribute its predefined Gameplay Tag
	 * so the Wrapper and UI Manager can quickly query what is active and react accordingly.
	 *
	 * @return Container of Gameplay Tags representing active UI states/widgets currently tracked by the UI Manager.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		meta=(Categories="Mountea_Inventory.WidgetState,State"),
		DisplayName="Get Widget States")
	FGameplayTagContainer GetWidgetStates() const;
	virtual FGameplayTagContainer GetWidgetStates_Implementation() const = 0;

	/**
	 * Overwrites UI Manager state tracking with a new tag container.
	 *
	 * This function replaces the current tracked Widget State tags with the provided container.
	 * It exists primarily as a write-back mechanism for systems that manipulate state tags in bulk
	 * (including helper functions such as AddWidgetStateTag, RemoveWidgetStateTag, ClearWidgetStateTags
	 * or AppendWidgetStateTags).
	 *
	 * @note
	 * - This function only updates state tracking. It should not be assumed to create/destroy widgets by itself.
	 * - Implementations should ensure the stored state remains consistent with actual active widgets.
	 *
	 * @param NewStates Container of Gameplay Tags that will become the UI Manager's active widget states.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Set Widget States")
	void SetWidgetStates(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& NewStates);
	virtual void SetWidgetStates_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& NewStates) = 0;

	/**
	 * Adds a new Widget State tag to the target UI Manager's active state container.
	 *
	 * Expected flow:
	 * - A child widget (e.g., ModalWindow) is created/added to the UI Manager.
	 * - That widget provides its state tag definition.
	 * - UI Manager registers the tag so the system can track that the widget is now active.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to add.
	 * @return True if the tag was added (was not present before). False if invalid or already present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Add Widget State Tag")
	bool AddWidgetStateTag(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag);
	virtual bool AddWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag) = 0;

	/**
	 * Removes an existing Widget State tag from the target UI Manager's active state container.
	 *
	 * This should be called when a UI element is removed/hidden/destroyed so the Wrapper no longer reports
	 * it as active, preventing stale state and enabling correct UI Manager decisions.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to remove.
	 * @return True if the tag was removed. False if invalid or not found.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Remove Widget State Tag")
	bool RemoveWidgetStateTag(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag);
	virtual bool RemoveWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag) = 0;

	/**
	 * Checks whether the target UI Manager currently contains a given Widget State tag.
	 *
	 * Used for querying whether a particular UI element/state is currently active within the Wrapper.
	 * Example: prevent opening another modal if "UI.Modal.Active" is already present, or change input rules.
	 *
	 * @param Tag The Gameplay Tag describing the widget state to check.
	 * @param bExactMatch If true, requires an exact tag match. If false, allows hierarchical matching
	 *                    (e.g., checking "UI.Modal" would match "UI.Modal.Active").
	 * @return True if the UI Manager currently reports the tag as active; otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Has Widget State Tag")
	bool HasWidgetStateTag(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag, 
		bool bExactMatch) const;
	virtual bool HasWidgetStateTag_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& Tag, 
		bool bExactMatch) const = 0;

	/**
	 * Clears all Widget State tags tracked by the target UI Manager.
	 *
	 * Typically used during full teardown/reset scenarios such as RemoveWrapperWidget, rebuilding the UI,
	 * or when the UI Manager needs to force the Wrapper into a clean baseline state.
	 *
	 * Note:
	 * - This does not automatically destroy UI widgets by itself. It only clears the tracked state tags.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Clear Widget State Tags")
	void ClearWidgetStateTags();
	virtual void ClearWidgetStateTags_Implementation() = 0;

	/**
	 * Appends multiple Widget State tags to the target UI Manager's active state container.
	 *
	 * Useful when adding a composite widget (or UI mode) that activates multiple tracked states at once,
	 * or when synchronizing the UI Manager to a known set of states provided by another system.
	 *
	 * @param TagsToAppend Container of Gameplay Tags to add to the UI Manager's active state container.
	 * @return True if at least one new tag was added. False if empty or all tags were already present.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|State",
		DisplayName="Append Widget State Tags")
	bool AppendWidgetStateTags(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& TagsToAppend);
	virtual bool AppendWidgetStateTags_Implementation(UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTagContainer& TagsToAppend) = 0;


	// --- Events
	
	virtual FInventoryCategorySelected& GetOnCategorySelectedHandle() = 0;
	virtual FInventoryItemSelected& GetOnItemSelectedHandle() = 0;
};
