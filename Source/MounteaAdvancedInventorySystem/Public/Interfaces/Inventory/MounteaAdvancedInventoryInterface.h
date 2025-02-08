// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryItem.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryInterface.generated.h"

struct FInventoryNotificationData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const FInventoryItem&, AddedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const FInventoryItem&, RemovedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemQuantityChanged, const FInventoryItem&, Item, int32, OldQuantity, int32, NewQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemDurabilityChanged, const FInventoryItem&, Item, float, OldDurability, float, NewDurability);


// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// --- Actor functions ------------------------------

	/**
	 * Returns the owning actor for this Dialogue Inventory Component.
	 *
	 * @return The owning actor for this Dialogue Inventory Component.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	// --- Core Inventory Functions ------------------------------

	/**
	* Adds an item to the inventory
	* @param Item The item to add
	* @param bAutoStack If true, will try to stack with existing items
	* @return True if item was added successfully
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool AddItem(const FInventoryItem& Item, bool bAutoStack = true);
	virtual bool AddItem_Implementation(const FInventoryItem& Item, bool bAutoStack = true) = 0;
	
	/**
	* Removes an item from inventory
	* @param ItemGuid GUID of item to remove
	* @return True if item was removed
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management") 
	bool RemoveItem(const FGuid& ItemGuid);
	virtual bool RemoveItem_Implementation(const FGuid& ItemGuid) = 0;

	/**
	* Checks if can add item to inventory
	* @param Item Item to check
	* @return True if item can be added
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool CanAddItem(const FInventoryItem& Item) const;
	virtual bool CanAddItem_Implementation(const FInventoryItem& Item) const = 0;

	/**
	* Finds item by GUID
	* @param ItemGuid GUID to search for
	* @return Found item or nullptr
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	FInventoryItem FindItem(const FGuid& ItemGuid);
	virtual FInventoryItem FindItem_Implementation(const FGuid& ItemGuid) = 0;

	/**
	* Gets all items in inventory
	* @return Array of inventory items
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	TArray<FInventoryItem> GetAllItems() const;
	virtual TArray<FInventoryItem> GetAllItems_Implementation() const = 0;

	/**
	* Clears all items from inventory
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	void ClearInventory();
	virtual void ClearInventory_Implementation() = 0;

	// --- Item Stack Management ------------------------------

	/**
	* Increases item quantity
	* @param ItemGuid Target item GUID
	* @param Amount Amount to increase
	* @return True if successful
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Stack")
	bool IncreaseItemQuantity(const FGuid& ItemGuid, int32 Amount = 1);
	virtual bool IncreaseItemQuantity_Implementation(const FGuid& ItemGuid, int32 Amount = 1) = 0;
	
	/**
	* Decreases item quantity
	* @param ItemGuid Target item GUID
	* @param Amount Amount to decrease
	* @return True if successful
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Stack")
	bool DecreaseItemQuantity(const FGuid& ItemGuid, int32 Amount = 1);
	virtual bool DecreaseItemQuantity_Implementation(const FGuid& ItemGuid, int32 Amount = 1) = 0;

	/**
	* Splits item stack into two
	* @param ItemGuid Source item GUID
	* @param SplitAmount Amount to split off
	* @return New item from split
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Stack")
	FInventoryItem SplitItemStack(const FGuid& ItemGuid, int32 SplitAmount);
	virtual FInventoryItem SplitItemStack_Implementation(const FGuid& ItemGuid, int32 SplitAmount) = 0;

	// --- Durability Management ------------------------------

	/**
	* Modifies item durability
	* @param ItemGuid Target item GUID
	* @param DeltaDurability Change in durability
	* @return True if successful
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Durability")
	bool ModifyItemDurability(const FGuid& ItemGuid, float DeltaDurability);
	virtual bool ModifyItemDurability_Implementation(const FGuid& ItemGuid, float DeltaDurability) = 0;
	
	// --- Notification Management ------------------------------

	/**
	 * 
	 * @param Notification 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Notifications")
	void ProcessInventoryNotification(const FInventoryNotificationData& Notification);
	virtual void ProcessInventoryNotification_Implementation(const FInventoryNotificationData& Notification) = 0;
};
