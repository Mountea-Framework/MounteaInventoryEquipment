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
class UCommonActivatableWidget;
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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const = 0;

	/**
	 * Sets the parent inventory for this UI.
	 * @param NewParentInventory - The inventory interface to associate.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) = 0;
	
	/**
	 * Creates the inventory UI.
	 * @return True if UI was successfully created, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	bool CreateWrapperWidget();
	virtual bool CreateWrapperWidget_Implementation() = 0;

	/**
	 * Returns the inventory UI.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	UUserWidget* GetWrapperWidget() const;
	virtual UUserWidget* GetWrapperWidget_Implementation() const = 0;

	/**
	 * Removes the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main")
	void RemoveWrapperWidget();
	virtual void RemoveWrapperWidget_Implementation() = 0;
	
	/**
	 * Creates the inventory UI.
	 * @return True if UI was successfully created, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Inventory")
	bool CreateInventoryWidget();
	virtual bool CreateInventoryWidget_Implementation() = 0;

	/**
	 * Returns the inventory UI.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Inventory")
	UUserWidget* GetInventoryWidget() const;
	virtual UUserWidget* GetInventoryWidget_Implementation() const = 0;
	
	/**
	 * Sets the inventory UI from outside.
	 * @return True if UI was successfully set, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Inventory")
	bool SetInventoryWidget(UUserWidget* NewInventoryWidget);
	virtual bool SetInventoryWidget_Implementation(UUserWidget* NewInventoryWidget) = 0;

	/**
	 * Removes the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Inventory")
	void RemoveInventoryWidget();
	virtual void RemoveInventoryWidget_Implementation() = 0;
	
	// --- Notification

	/**
	 * Retrieves the notification container widget.
	 * @return A pointer to the notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Notifications")
	UUserWidget* GetNotificationContainer() const;
	virtual UUserWidget* GetNotificationContainer_Implementation() const = 0;

	/**
	 * Sets the notification container widget.
	 * @param NewNotificationContainer - The new notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Notifications")
	void SetNotificationContainer(UUserWidget* NewNotificationContainer);
	virtual void SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer) = 0;

	/**
	 * Creates a new inventory notification.
	 * @param NotificationData - The data defining the notification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Notifications")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	/**
	 * Removes all active inventory notifications.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Main|Notifications")
	void RemoveInventoryNotifications();
	virtual void RemoveInventoryNotifications_Implementation() = 0;

	// --- Categories
	
	/**
	 * Handles the selection of a category in the inventory UI.
	 * @param SelectedCategoryId The ID of the selected category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Category")
	void CategorySelected(const FString& SelectedCategoryId);
	virtual void CategorySelected_Implementation(const FString& SelectedCategoryId) = 0;

	/**
	 * Retrieves the currently selected category ID.
	 * @return The ID of the currently selected category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Category")
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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Item")
	UWidget* GetActiveItemWidget() const;
	virtual UWidget* GetActiveItemWidget_Implementation() const = 0;

	/**
	 * Sets the active item widget in the UI.
	 *
	 * @param NewActiveItemWidget The new widget to be set as the active item widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Item")
	void SetActiveItemWidget(UWidget* NewActiveItemWidget);
	virtual void SetActiveItemWidget_Implementation(UWidget* NewActiveItemWidget) = 0;

	/**
	 * Processes an item that has been added to the inventory.
	 * @param AddedItem The item that has been added to the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void ProcessItemAdded(const FInventoryItem& AddedItem);
	virtual void ProcessItemAdded_Implementation(const FInventoryItem& AddedItem) = 0;

	/**
	 * Processes an item that has been removed from the inventory.
	 * @param RemovedItem The item that has been removed from the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void ProcessItemRemoved(const FInventoryItem& RemovedItem);
	virtual void ProcessItemRemoved_Implementation(const FInventoryItem& RemovedItem) = 0;

	/**
	 * Processes an item that has been modified in the inventory.
	 * @param ModifiedItem The item that has been modified in the inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void ProcessItemModified(const FInventoryItem& ModifiedItem);
	virtual void ProcessItemModified_Implementation(const FInventoryItem& ModifiedItem) = 0;
	
	/**
	 * Handles the selection of an item in the inventory UI.
	 * @param SelectedItem The GUID of the selected item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	void ItemSelected(const FGuid& SelectedItem);
	virtual void ItemSelected_Implementation(const FGuid& SelectedItem) = 0;

	/**
	 * Retrieves the currently active item GUID.
	 * @return The GUID of the currently active item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Item")
	FGuid GetActiveItemGuid() const;
	virtual FGuid GetActiveItemGuid_Implementation() const = 0;
	
	// --- Items

	/**
	 * Retrieves local custom items map.
	 * 
	 * @return Custom Items Mpa if any specified.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap() const;
	virtual TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap_Implementation() const = 0;
	
	/**
 * Adds (or replaces) a single entry in the custom items map.
 *
 * @param ItemTag   Gameplay tag representing the custom item key.
 * @param ItemId    Guid value associated with the item.
 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void AddCustomItemToMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual void AddCustomItemToMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Appends entries from another map into the custom items map.
	 * Existing keys will be overwritten with new values.
	 *
	 * @param OtherItems   Map of custom items to merge in.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void AppendCustomItemsMap(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems);
	virtual void AppendCustomItemsMap_Implementation(const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems) = 0;

	/**
	 * Clears all entries from the custom items map.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
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
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	bool RemoveCustomItemFromMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual bool RemoveCustomItemFromMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

	/**
	 * Validates whether the provided Item guid is stored in a map for specified tag.
	 * Example:
	 * → Is Item in Favorites
	 * 
	 * @param ItemTag Tag which defines the key, like "Favorite"
	 * @param ItemId Item guid to search for
	 * @return True if item is stored in such container, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	bool IsItemStoredInCustomMap(const FGameplayTag& ItemTag, const FGuid& ItemId);
	virtual bool IsItemStoredInCustomMap_Implementation(const FGameplayTag& ItemTag, const FGuid& ItemId) = 0;

		
	// --- Slots

	/**
	 * Retrieves the saved inventory grid slots.
	 *
	 * This function provides a set of saved inventory grid slots, represented as FMounteaInventoryGridSlot objects.
	 * It is used to maintain and manage the inventory layout, allowing you to save and restore UI states.
	 *
	 * @return A set of saved FMounteaInventoryGridSlot instances representing the current saved state of the inventory grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Slots")
	TSet<FMounteaInventoryGridSlot> GetSavedSlots() const;
	virtual TSet<FMounteaInventoryGridSlot> GetSavedSlots_Implementation() const = 0;

	/**
	 * Adds a single grid slot to the inventory.
	 *
	 * This function adds a single FMounteaInventoryGridSlot to the grid, allowing an item to be placed into a new slot.
	 * 
	 * @param SlotData The data of the slot being added to the grid.
	 */
	UFUNCTION(BlueprintNativeEvent,  Category="Mountea|Inventory & Equipment|Inventory|UI|Slots")
	void AddSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void AddSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	/**
	 * Removes a single grid slot from the inventory.
	 *
	 * This function removes a specific FMounteaInventoryGridSlot from the grid, effectively clearing that slot.
	 *
	 * @param SlotData The data of the slot being removed from the grid.
	 */
	UFUNCTION(BlueprintNativeEvent,  Category="Mountea|Inventory & Equipment|Inventory|UI|Slots")
	void RemoveSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void RemoveSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	/**
	 * Adds multiple grid slots to the inventory.
	 *
	 * This function adds a collection of FMounteaInventoryGridSlots to the grid, expanding the inventory layout.
	 *
	 * @param SlotData A set of FMounteaInventoryGridSlot instances to be added to the grid.
	 */
	UFUNCTION(BlueprintNativeEvent,  Category="Mountea|Inventory & Equipment|Inventory|UI|Slots")
	void AddSlots(const TSet<FMounteaInventoryGridSlot>& SlotData);
	virtual void AddSlots_Implementation(const TSet<FMounteaInventoryGridSlot>& SlotData) = 0;

	/**
	 * Removes multiple grid slots from the inventory.
	 *
	 * This function removes a collection of FMounteaInventoryGridSlots from the grid, clearing those slots.
	 *
	 * @param SlotData A set of FMounteaInventoryGridSlot instances to be removed from the grid.
	 */
	UFUNCTION(BlueprintNativeEvent,  Category="Mountea|Inventory & Equipment|Inventory|UI|Slots")
	void RemoveSlots(const TSet<FMounteaInventoryGridSlot>& SlotData);
	virtual void RemoveSlots_Implementation(const TSet<FMounteaInventoryGridSlot>& SlotData) = 0;

	/**
	 * Updates an existing grid slot in the inventory.
	 *
	 * This function updates the data of an existing FMounteaInventoryGridSlot, modifying the information of the slot.
	 * 
	 * @param SlotData The updated data of the grid slot to be modified.
	 */
	UFUNCTION(BlueprintNativeEvent,  Category="Mountea|Inventory & Equipment|Inventory|UI|Slots")
	void UpdateSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void UpdateSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	// --- Events
	
	virtual FInventoryCategorySelected& GetOnCategorySelectedHandle() = 0;
	virtual FInventoryItemSelected& GetOnItemSelectedHandle() = 0;
};
