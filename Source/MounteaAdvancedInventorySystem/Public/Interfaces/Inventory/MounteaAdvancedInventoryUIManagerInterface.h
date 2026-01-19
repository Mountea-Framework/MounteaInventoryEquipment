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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Category")
	void CategorySelected(const FString& SelectedCategoryId);
	virtual void CategorySelected_Implementation(const FString& SelectedCategoryId) = 0;

	/**
	 * Retrieves the currently selected category ID.
	 * @return The ID of the currently selected category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Category")
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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Item")
	void ProcessItemAdded(const FInventoryItem& AddedItem);
	virtual void ProcessItemAdded_Implementation(const FInventoryItem& AddedItem) = 0;

	/**
	 * Processes an item that has been removed from the inventory.
	 * @param RemovedItem The item that has been removed from the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Item")
	void ProcessItemRemoved(const FInventoryItem& RemovedItem);
	virtual void ProcessItemRemoved_Implementation(const FInventoryItem& RemovedItem) = 0;

	/**
	 * Processes an item that has been modified in the inventory.
	 * @param ModifiedItem The item that has been modified in the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Item")
	void ProcessItemModified(const FInventoryItem& ModifiedItem);
	virtual void ProcessItemModified_Implementation(const FInventoryItem& ModifiedItem) = 0;
	
	/**
	 * Handles the selection of an item in the inventory UI.
	 * @param SelectedItem The GUID of the selected item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Item")
	void ItemSelected(const FGuid& SelectedItem);
	virtual void ItemSelected_Implementation(const FGuid& SelectedItem) = 0;

	/**
	 * Retrieves the currently active item GUID.
	 * @return The GUID of the currently active item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Item")
	FGuid GetActiveItemGuid() const;
	virtual FGuid GetActiveItemGuid_Implementation() const = 0;
	
	// --- Items

	/**
	 * Retrieves local custom items map.
	 * 
	 * @return Custom Items Mpa if any specified.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Items")
	TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap() const;
	virtual TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap_Implementation() const = 0;
	
	/**
	 * Adds (or replaces) a single entry in the custom items map.
	 *
	 * @param ItemTag   Gameplay tag representing the custom item key.
	 * @param ItemId    Guid value associated with the item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Items")
	void AddCustomItemToMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual void AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Appends entries from another map into the custom items map.
	 * Existing keys will be overwritten with new values.
	 *
	 * @param OtherItems   Map of custom items to merge in.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Items")
	void AppendCustomItemsMap(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems);
	virtual void AppendCustomItemsMap_Implementation(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems) = 0;

	/**
	 * Clears all entries from the custom items map.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Items")
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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Items")
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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Manager|Items")
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
	
	// --- Selectable Actions
	
	/**
	 * Returns a snapshot of the currently queued Item Actions waiting to be processed.
	 *
	 * ItemActionsQueue order reflects processing order (FIFO), where the first element is the next action to execute.
	 *
	 * @return Array of queued Item Actions.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	TArray<UMounteaSelectableInventoryItemAction*> GetItemActionsQueue() const;
	virtual TArray<UMounteaSelectableInventoryItemAction*> GetItemActionsQueue_Implementation() const = 0;

	/**
	 * Enqueues a single Item Action to be executed by the UI Manager.
	 *
	 * The provided Payload is passed forward to the action execution and can represent any context data
	 * required for processing (such as the targeted item, owning inventory, UI widget, etc.).
	 *
	 * @param ItemAction Item Action to enqueue.
	 * @param Payload Optional context object passed to the queued action.
	 *
	 * @return True if the action was successfully enqueued, false otherwise (invalid action, queue disabled, etc.).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	bool EnqueueItemAction(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload);
	virtual bool EnqueueItemAction_Implementation(UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload) = 0;

	/**
	 * Enqueues multiple Item Actions to be executed by the UI Manager in the provided order.
	 *
	 * All actions receive the same Payload instance. The queue order reflects the order within the input array.
	 *
	 * @param ItemActions Item Actions to enqueue.
	 * @param Payload Optional context object passed to each queued action.
	 *
	 * @return True if all actions were successfully enqueued, false otherwise (some actions may be rejected).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	bool EnqueueItemActions(TArray<UMounteaSelectableInventoryItemAction*>& ItemActions, UObject* Payload);
	virtual bool EnqueueItemActions_Implementation(TArray<UMounteaSelectableInventoryItemAction*>& ItemActions, UObject* Payload) = 0;
	
	/**
	 * Empties the queued Item Actions waiting for execution.
	 *
	 * This does not necessarily cancel the currently executing action unless the implementation explicitly does so.
	 * Recommended behavior is to leave the current action intact and clear only pending actions.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	void EmptyItemActionsQueue();
	virtual void EmptyItemActionsQueue_Implementation()= 0;;

	/**
	 * Pauses processing of the Item Action queue.
	 *
	 * When paused, no new actions are started, but the currently running action may continue depending on
	 * implementation and action design. The queue remains paused until ResumeItemActionsQueue is called.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	void PauseItemActionsQueue();
	virtual void PauseItemActionsQueue_Implementation() = 0;

	/**
	 * Resumes processing of the Item Action queue after it has been paused.
	 *
	 * If no action is currently running, resuming should typically trigger processing of the next queued action.
	 *
	 * @return True if queue was resumed successfully, false otherwise (queue not paused, no valid state, etc.).
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	bool ResumeItemActionsQueue();
	virtual bool ResumeItemActionsQueue_Implementation() = 0;
	
	/**
	 * Starts processing of the Item Action queue.
	 *
	 * If no action is currently in the queue the queue won't start processing anything and will idle itself.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|ItemActions")
	void StartItemActionsQueue();
	virtual void StartItemActionsQueue_Implementation() = 0;

	// --- Events
	
	virtual FInventoryCategorySelected& GetOnCategorySelectedHandle() = 0;
	virtual FInventoryItemSelected& GetOnItemSelectedHandle() = 0;
};
