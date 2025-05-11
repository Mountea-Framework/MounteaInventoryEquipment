// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryUIInterface.generated.h"

struct FInventoryItem;
struct FInventoryNotificationData;
class IMounteaAdvancedInventoryInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryCategorySelected, const FString&, SelectedCategoryId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemSelected, const FGuid&, SelectedItemId);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for advanced inventory UI interactions.
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryUIInterface
{
	GENERATED_BODY()

public:
	
	// --- Inventory  ------------------------------

	/**
	 * Retrieves the parent inventory associated with this UI.
	 * @return The parent inventory interface.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const = 0;

	/**
	 * Sets the parent inventory for this UI.
	 * @param NewParentInventory - The inventory interface to associate.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) = 0;

	/**
	 * Retrieves the visibility state of the main UI.
	 *
	 * This function determines the current visibility status of the main UI
	 * and returns an appropriate ESlateVisibility value.
	 *
	 * @return The visibility state of the main UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	ESlateVisibility GetMainUIVisibility() const;
	virtual ESlateVisibility GetMainUIVisibility_Implementation() const = 0;

	/**
	 * Updates the visibility state of the main  UI.
	 *
	 * @param NewVisibility The new visibility state of the UI, defined as an ESlateVisibility value.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetMainUIVisibility(const ESlateVisibility NewVisibility);
	virtual void SetMainUIVisibility_Implementation(const ESlateVisibility NewVisibility) = 0;

	/**
	 * Creates the inventory UI.
	 * @return True if UI was successfully created, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	bool CreateInventoryUIWrapper();
	virtual bool CreateInventoryUIWrapper_Implementation() = 0;

	/**
	 * Returns the inventory UI.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	UUserWidget* GetInventoryUIWrapper() const;
	virtual UUserWidget* GetInventoryUIWrapper_Implementation() const = 0;

	/**
	 * Removes the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void RemoveInventoryUIWrapper();
	virtual void RemoveInventoryUIWrapper_Implementation() = 0;

	/**
	 * Sets the visibility of the inventory UI.
	 * @param bShowInventory The new visibility state of the UI.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetInventoryUIWrapperVisibility(const bool bShowInventory);
	virtual void SetInventoryUIWrapperVisibility_Implementation(const bool bShowInventory) = 0;

	// --- Notification  ------------------------------

	/**
	 * Retrieves the notification container widget.
	 * @return A pointer to the notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	UUserWidget* GetNotificationContainer() const;
	virtual UUserWidget* GetNotificationContainer_Implementation() const = 0;

	/**
	 * Sets the notification container widget.
	 * @param NewNotificationContainer - The new notification container widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void SetNotificationContainer(UUserWidget* NewNotificationContainer);
	virtual void SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer) = 0;

	/**
	 * Creates a new inventory notification.
	 * @param NotificationData - The data defining the notification.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	/**
	 * Removes all active inventory notifications.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void RemoveInventoryNotifications();
	virtual void RemoveInventoryNotifications_Implementation() = 0;

	// --- Categories
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void CategorySelected(const FString& SelectedCategoryId);
	virtual void CategorySelected_Implementation(const FString& SelectedCategoryId) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	FString GetSelectedCategoryId() const;
	virtual FString GetSelectedCategoryId_Implementation() const = 0;

	// --- Items

	/**
	 * 
	 * @param AddedItem 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void ProcessItemAdded(const FInventoryItem& AddedItem);
	virtual void ProcessItemAdded_Implementation(const FInventoryItem& AddedItem) = 0;

	/**
	 * 
	 * @param RemovedItem 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void ProcessItemRemoved(const FInventoryItem& RemovedItem);
	virtual void ProcessItemRemoved_Implementation(const FInventoryItem& RemovedItem) = 0;

	/**
	 * 
	 * @param ModifiedItem 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void ProcessItemModified(const FInventoryItem& ModifiedItem);
	virtual void ProcessItemModified_Implementation(const FInventoryItem& ModifiedItem) = 0;
	
	/**
	 * 
	 * @param SelectedItem 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void ItemSelected(const FGuid& SelectedItem);
	virtual void ItemSelected_Implementation(const FGuid& SelectedItem) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	FGuid GetActiveItemGuid() const;
	virtual FGuid GetActiveItemGuid_Implementation() const = 0;

	/**
	 * Retrieves the saved inventory grid slots.
	 *
	 * This function provides a set of saved inventory grid slots, represented as FMounteaInventoryGridSlot objects.
	 * It is used to maintain and manage the inventory layout, allowing you to save and restore UI states.
	 *
	 * @return A set of saved FMounteaInventoryGridSlot instances representing the current saved state of the inventory grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	TSet<FMounteaInventoryGridSlot> GetSavedSlots() const;
	virtual TSet<FMounteaInventoryGridSlot> GetSavedSlots_Implementation() const = 0;

	/**
	 * Adds a single grid slot to the inventory.
	 *
	 * This function adds a single FMounteaInventoryGridSlot to the grid, allowing an item to be placed into a new slot.
	 * 
	 * @param SlotData The data of the slot being added to the grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void AddSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void AddSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	/**
	 * Removes a single grid slot from the inventory.
	 *
	 * This function removes a specific FMounteaInventoryGridSlot from the grid, effectively clearing that slot.
	 *
	 * @param SlotData The data of the slot being removed from the grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void RemoveSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void RemoveSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	/**
	 * Adds multiple grid slots to the inventory.
	 *
	 * This function adds a collection of FMounteaInventoryGridSlots to the grid, expanding the inventory layout.
	 *
	 * @param SlotData A set of FMounteaInventoryGridSlot instances to be added to the grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void AddSlots(const TSet<FMounteaInventoryGridSlot>& SlotData);
	virtual void AddSlots_Implementation(const TSet<FMounteaInventoryGridSlot>& SlotData) = 0;

	/**
	 * Removes multiple grid slots from the inventory.
	 *
	 * This function removes a collection of FMounteaInventoryGridSlots from the grid, clearing those slots.
	 *
	 * @param SlotData A set of FMounteaInventoryGridSlot instances to be removed from the grid.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void RemoveSlots(const TSet<FMounteaInventoryGridSlot>& SlotData);
	virtual void RemoveSlots_Implementation(const TSet<FMounteaInventoryGridSlot>& SlotData) = 0;

	/**
	 * Updates an existing grid slot in the inventory.
	 *
	 * This function updates the data of an existing FMounteaInventoryGridSlot, modifying the information of the slot.
	 * 
	 * @param SlotData The updated data of the grid slot to be modified.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI")
	void UpdateSlot(const FMounteaInventoryGridSlot& SlotData);
	virtual void UpdateSlot_Implementation(const FMounteaInventoryGridSlot& SlotData) = 0;

	virtual FInventoryCategorySelected& GetOnCategorySelectedHandle() = 0;
	virtual FInventoryItemSelected& GetOnItemSelectedHandle() = 0;
};
