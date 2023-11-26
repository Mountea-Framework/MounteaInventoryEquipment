// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryInterface.generated.h"

struct FItemTransfer;
class IMounteaInventoryInstancedItemInterface;
class UMounteaInstancedItem;
class UMounteaBaseUserWidget;
class UMounteaInventoryConfig;
class UMounteaTransactionPayload;
class UMounteaInventoryItemBase;
class UMounteaInventoryItemAction;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const FInventoryUpdateResult&, UpdateContext);

/**
 * Mountea Inventory Interface.
 *
 * Provides functions to manage Inventory, for example:
 * * Provide info about Inventory content
 * * Add new Items
 * * Remove Items
 * * Request Item Actions
 *
 * Inventory is player-specific and thus should be attached to Player Controller.
 * If player inventory is tied to Player pawn (if player dies, his loot stays on his corpse), then use Player Pawn instead.
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryInterface
{
	GENERATED_BODY()

public:

	/**
	 * Retrieves the class type of the user interface associated with this inventory.
	 * @return The class type of the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TSubclassOf<UMounteaBaseUserWidget> GetInventoryUIClass() const;
	virtual TSubclassOf<UMounteaBaseUserWidget> GetInventoryUIClass_Implementation() const = 0;

	/**
	 * Sets the class type for the inventory's user interface.
	 * @param NewInventoryUIClass The new class to be set for the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetInventoryUIClass(TSubclassOf<UMounteaBaseUserWidget> NewInventoryUIClass);
	virtual void SetInventoryUIClass_Implementation(TSubclassOf<UMounteaBaseUserWidget> NewInventoryUIClass) = 0;

	/**
	 * Sets the actual instance of the inventory's UI widget.
	 * @param NewUI The new widget instance to be set as the inventory UI.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetInventoryUI(UMounteaBaseUserWidget* NewUI);
	virtual void SetInventoryUI_Implementation(UMounteaBaseUserWidget* NewUI) = 0;

	/**
	 * Retrieves the current instance of the inventory's UI widget.
	 * @return The current instance of the inventory UI widget.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaBaseUserWidget* GetInventoryUI() const;
	virtual UMounteaBaseUserWidget* GetInventoryUI_Implementation() const = 0;

	/**
	 * Adds an item to the inventory, handling different scenarios such as adding a new item,
	 * stacking with existing ones, or creating a new item when only a part of it can be added.
	 * It manages the item quantity, updates inventory slots, and broadcasts relevant events.
	 * 
	 * @param Item The item to add to the inventory.
	 * @param Quantity The quantity of the item to be added.
	 * @return A structure containing details about the outcome of the addition process.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult AddItemToInventory(UMounteaInstancedItem* Item, const int32& Quantity = 1);
	virtual FInventoryUpdateResult AddItemToInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) = 0;

	/**
	 * Adds a bunch of items to the inventory, handling different scenarios such as adding a new item,
	 * stacking with existing ones, or creating a new item when only a part of it can be added.
	 * It manages the item quantity, updates inventory slots, and broadcasts relevant events.
	 * 
	 * @param Items The map of  items and their quantities to add to the inventory.
	 * @param TransactionTypeFlags Bitmasks that allows for multiple flags to be combined and checked easily. 
	 * @return A list of structures containing details about the outcome of the addition processes.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FInventoryUpdateResult> AddItemsToInventory(const TMap<UMounteaInstancedItem*, int32>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags);
	virtual TArray<FInventoryUpdateResult> AddItemsToInventory_Implementation(const TMap<UMounteaInstancedItem*, int32>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags = 10) = 0;

	/**
	 * Removes a specific item from the inventory, handling the cleanup and updating of inventory slots.
	 * It ensures that all relevant data structures are updated and broadcasts the item removal event.
	 * 
	 * @param Item The item to be removed from the inventory.
	 * @return A structure containing details about the outcome of the removal process.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult RemoveItemFromInventory(UMounteaInstancedItem* Item);
	virtual FInventoryUpdateResult RemoveItemFromInventory_Implementation(UMounteaInstancedItem* Item) = 0;

	/**
	 * Removes a bunch of specific items from the inventory, handling the cleanup and updating of inventory slots.
	 * It ensures that all relevant data structures are updated and broadcasts the item removal event.
	 * 
	 * @param Items The list of items to be removed from the inventory.
	 * @param TransactionTypeFlags Bitmasks that allows for multiple flags to be combined and checked easily. 
	 * @return A list of structures containing details about the outcome of the removal process.
	 */
	TArray<FInventoryUpdateResult> RemoveItemsFromInventory(const TArray<UMounteaInstancedItem*>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags);
	virtual TArray<FInventoryUpdateResult> RemoveItemsFromInventory_Implementation(const TArray<UMounteaInstancedItem*>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags = 10) = 0;
	
	/**
	 * Reduces the quantity of a specific item in the inventory. If the item's quantity
	 * falls to zero or below, it triggers the item's removal from the inventory.
	 * It updates the relevant inventory slots and broadcasts update or removal events.
	 * 
	 * @param Item The item for which the quantity should be reduced.
	 * @param Quantity The amount by which to reduce the item's quantity.
	 * @return A structure containing details about the outcome of the reduction process.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult ReduceItemInInventory(UMounteaInstancedItem* Item, const int32& Quantity = 1);
	virtual FInventoryUpdateResult ReduceItemInInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) = 0;

	/**
	 * Reduces the quantity of a specific items in the inventory. If any of those item's quantity
	 * falls to zero or below, it triggers the item's removal from the inventory.
	 * It updates the relevant inventory slots and broadcasts update or removal events.
	 * 
	 * @param Items The list item for which the quantity should be reduced.
	 * @param TransactionTypeFlags Bitmasks that allows for multiple flags to be combined and checked easily. 
	 * @return A list of structures containing details about the outcome of the reduction process.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FInventoryUpdateResult> ReduceItemsInInventory(const TMap<UMounteaInstancedItem*, int32>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags);
	virtual TArray<FInventoryUpdateResult> ReduceItemsInInventory_Implementation(const TMap<UMounteaInstancedItem*, int32>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags = 10) = 0;
	
	/**
	 * Determines if an item can be added to the inventory.
	 * @param Item The item to be added.
	 * @param Quantity The quantity of the item to be added.
	 * @return True if the item can be added, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanAddItem(const FItemTransfer& Item) const;
	virtual bool CanAddItem_Implementation(const FItemTransfer& Item) const = 0;

	/**
	 * Determines if an item can be removed from the inventory.
	 * @param Item The item to be removed.
	 * @return True if the item can be removed, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanRemoveItem(UMounteaInstancedItem* Item) const;
	virtual bool CanRemoveItem_Implementation(UMounteaInstancedItem* Item) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanReduceItem(const FItemTransfer& Item) const;
	virtual bool CanReduceItem_Implementation(const FItemTransfer& Item) const = 0;
	
	/**
	 * Searches the inventory for a single item that matches the given search filter.
	 * @param SearchFilter The criteria used to search for the item.
	 * @return The first item that matches the search criteria, if found.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaInstancedItem* SearchSingleItem(const FItemRetrievalFilter& SearchFilter) const;
	virtual UMounteaInstancedItem* SearchSingleItem_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;

	/**
	 * Searches the inventory for multiple items that match the given search filter.
	 * @param SearchFilter The criteria used to search for items.
	 * @return An array of items that match the search criteria.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInstancedItem*> SearchMultipleItems(const FItemRetrievalFilter& SearchFilter) const;
	virtual TArray<UMounteaInstancedItem*> SearchMultipleItems_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;

	/**
	 * Returns an array of Instanced Item objects.
	 * This function returns all items in the inventory.
	 * For detailed search use `SearchMultipleItems`.
	 *
	 * @return An array of all inventory items.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInstancedItem*> GetItems() const;
	virtual TArray<UMounteaInstancedItem*> GetItems_Implementation() const = 0;
	
	/**
	 * Checks if the inventory contains any items that match the given search filter.
	 * @param SearchFilter The criteria used to search for items.
	 * @return True if any items matching the criteria are found, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasItem(const FItemRetrievalFilter& SearchFilter) const;
	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;

	/**
	 * Sets new gameplay tags for the inventory, replacing any existing tags.
	 * @param NewFlags The new tags to set for the inventory.
	 * @return True if the tags were successfully set, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool SetInventoryFlags(const FGameplayTagContainer& NewFlags);
	virtual bool SetInventoryFlags_Implementation(const FGameplayTagContainer& NewFlags) = 0;

	/**
	 * Adds a new gameplay tag to the inventory's existing tags.
	 * @param NewFlag The new tag to add to the inventory.
	 * @return True if the tag was successfully added, false if the tag was already present.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool SetInventoryFlag(const FGameplayTag& NewFlag);
	virtual bool SetInventoryFlag_Implementation(const FGameplayTag& NewFlag) = 0;

	/**
	* Gets the gameplay tags that represent the inventory's properties and state.
	* @return A container of gameplay tags for the inventory.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FGameplayTagContainer GetInventoryFlags() const;
	virtual FGameplayTagContainer GetInventoryFlags_Implementation() const = 0;

	/**
	 * Checks if the inventory has a specific gameplay tag.
	 * @param SearchedFlag The tag to search for in the inventory's gameplay tags.
	 * @param bSearchExact A boolean indicating whether to search for the exact tag or any matching tags.
	 * @return True if the inventory has the specified tag, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasFlag(const FGameplayTag& SearchedFlag, const bool bSearchExact = true);
	virtual bool HasFlag_Implementation(const FGameplayTag& SearchedFlag, const bool bSearchExact = true) = 0;

	/**
	 * Checks if the inventory has any or all of the specified gameplay tags.
	 * @param SearchedFlags The tags to search for in the inventory's gameplay tags.
	 * @param bSearchExact A boolean indicating whether to search for the exact tags or any matching tags.
	 * @param bSearchFast A boolean indicating whether to search for any match (true) or all matches (false).
	 * @return True if the inventory has the specified tags according to the search parameters, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasFlags(const FGameplayTagContainer& SearchedFlags, const bool bSearchExact = true, const bool bSearchFast = true);
	virtual bool HasFlags_Implementation(const FGameplayTagContainer& SearchedFlags, const bool bSearchExact = true, const bool bSearchFast = true) = 0;

	/**
	 * Retrieves the actor to which this inventory belongs.
	 * @return The actor that owns this Inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	/**
	 * Retrieves the inventory configuration object, optionally filtered by class.
	 * @param ClassFilter The class to filter the inventory configuration object by.
	 * @param bResult A reference to a boolean that will indicate whether the operation was successful.
	 * @return The inventory configuration object, if found and valid.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	UMounteaInventoryConfig* GetInventoryConfig( TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult) const;
	virtual UMounteaInventoryConfig* GetInventoryConfig_Implementation( TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult) const = 0;

	/**
	 * Retrieves the class of the inventory configuration object.
	 * @return The class of the inventory configuration object.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TSubclassOf<UMounteaInventoryConfig> GetInventoryConfigClass() const;
	virtual TSubclassOf<UMounteaInventoryConfig> GetInventoryConfigClass_Implementation() const = 0;

	/**
	 * Checks if the Inventory has authority within the networked game.
	 * @return True if the component has authority, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool DoesHaveAuthority() const;
	virtual bool DoesHaveAuthority_Implementation() const = 0;

	/**
	 * Requests a network refresh to ensure that the inventory's state is consistent across the network.
	 * This function is used to trigger a refresh or update of the inventory's replication state, ensuring
	 * that all clients have the latest information about the inventory contents.
	 *
	 * The function checks the network role and if it's not in a state of authority (e.g., on a client),
	 * it sends a request to the server to perform the actual refresh. If it is on the server, it directly
	 * increments a key used for tracking replication changes, which in turn triggers a network update.
	 *
	 * This is particularly useful in scenarios where immediate synchronization of inventory changes is crucial,
	 * such as after adding, removing, or modifying items in the inventory.
	 *
	 * Usage:
	 * Call this function after performing an operation that modifies the inventory (like adding or removing items)
	 * when you need to ensure that these changes are immediately reflected and synchronized across all connected clients.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void RequestNetworkRefresh();
	virtual void RequestNetworkRefresh_Implementation() = 0;
	
public:

	/**
	 * Retrieves a reference to the inventory updated event handle.
	 * This event is broadcasted when any change occurs in the inventory, 
	 * such as adding, removing, or updating items.
	 * 
	 * @return Reference to the inventory updated event delegate.
	 */
	virtual FOnInventoryUpdated& GetInventoryUpdatedHandle() = 0;
	/**
	 * Retrieves a reference to the item added event handle.
	 * This event is broadcasted specifically when a new item is successfully added to the inventory.
	 * @return Reference to the item added event delegate.
	 */
	virtual FOnInventoryUpdated& GetItemAddedHandle() = 0;
	/**
	 * Retrieves a reference to the item removed event handle.
	 * This event is broadcasted when an item is successfully removed from the inventory.
	 * @return Reference to the item removed event delegate.
	 */
	virtual FOnInventoryUpdated& GetItemRemovedHandle() = 0;
	/**
	 * Retrieves a reference to the item updated event handle.
	 * This event is broadcasted when an existing item in the inventory is modified, 
	 * such as a change in quantity or properties.
	 * 
	 * @return Reference to the item updated event delegate.
	 */
	virtual FOnInventoryUpdated& GetItemUpdatedHandle() = 0;

	/**
	 * Processes a specific action on an item within the inventory. This function manages how an item is manipulated or interacted with, 
	 * based on the action provided. It ensures that the action is executed correctly and the item is updated as needed.
	 *
	 * @param Action The action to be performed on the item. This could be any action derived from UMounteaInventoryItemAction, 
	 * such as using, equipping, or inspecting the item.
	 * @param Item The inventory item on which the action is to be performed. This should be a valid instance of UMounteaInstancedItem.
	 * @param Context Contextual information that may be needed to execute the action. This includes any dynamic parameters or conditions 
	 * required by the action.
	 *
	 * @return FInventoryUpdateResult structure containing the outcome of the action. This includes status codes, messages, and any 
	 * additional payload that might be relevant to the action's result.
	 *
	 * Example Usage:
	 * Let's say you have an action to consume a health potion. You would call this function with the consume action, the specific potion 
	 * item, and any necessary context for the action. The function then processes this action, updates the potion's quantity, and returns 
	 * the result of this operation.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult ProcessItemAction(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context);
	virtual FInventoryUpdateResult ProcessItemAction_Implementation(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context) = 0;
	
/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SaveInventory();
	virtual void SaveInventory_Implementation() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool LoadInventoryFromDataTable(const class UMounteaInventoryItemsTable* SourceTable);
	virtual bool LoadInventoryFromDataTable_Implementation(const class UMounteaInventoryItemsTable* SourceTable) = 0;
	
};
