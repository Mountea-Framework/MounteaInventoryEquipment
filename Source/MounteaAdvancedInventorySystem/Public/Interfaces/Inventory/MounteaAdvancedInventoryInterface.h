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
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryItem.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryInterface.generated.h"

class UMounteaInventoryItemTemplate;

/**
 * FInventoryItemSearchParams defines search criteria for locating inventory items.
 * Search parameters support multiple filtering methods including GUID, template, tags, category,
 * and rarity for flexible item queries within inventory systems.
 *
 * @see [Item Search](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/ItemSearch)
 * @see IMounteaAdvancedInventoryInterface
 * @see UMounteaInventoryItemTemplate
 */
USTRUCT(BlueprintType)
struct FInventoryItemSearchParams
{
	GENERATED_BODY()

	FInventoryItemSearchParams()
		: bSearchByGuid(false)
		, bSearchByTemplate(false)
		, bSearchByTags(false)
		, bRequireAllTags(false)
		, bSearchByCategory(false)
		, bSearchByRarity(false)
	{
	}

	explicit FInventoryItemSearchParams(const FGuid& InGuid)
		: bSearchByGuid(true)
		, ItemGuid(InGuid)
		, bSearchByTemplate(false)
		, bSearchByTags(false)
		, bRequireAllTags(false)
		, bSearchByCategory(false)
		, bSearchByRarity(false)
	{}

	explicit FInventoryItemSearchParams(UMounteaInventoryItemTemplate* const InTemplate)
		: bSearchByGuid(false)
		, bSearchByTemplate(true)
		, Template(InTemplate)
		, bSearchByTags(false)
		, bRequireAllTags(false)
		, bSearchByCategory(false)
		, bSearchByRarity(false)
	{}

	explicit FInventoryItemSearchParams(const FGameplayTagContainer& InTags, const bool bInRequireAllTags = false)	
		: bSearchByGuid(false)
		, bSearchByTemplate(false)
		, bSearchByTags(true)
		, Tags(InTags)
		, bRequireAllTags(bInRequireAllTags)
		, bSearchByCategory(false)
		, bSearchByRarity(false)
	{}

	FInventoryItemSearchParams(const FGuid& InGuid, UMounteaInventoryItemTemplate* const InTemplate, const FGameplayTagContainer& InTags, const bool bInRequireAllTags = false)
		: bSearchByGuid(true)
		, ItemGuid(InGuid)
		, bSearchByTemplate(true)
		, Template(InTemplate)
		, bSearchByTags(true)
		, Tags(InTags)
		, bRequireAllTags(bInRequireAllTags)
		, bSearchByCategory(false)
		, bSearchByRarity(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByGuid : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	FGuid ItemGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByTemplate : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	TObjectPtr<UMounteaInventoryItemTemplate> Template;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByTags : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bRequireAllTags : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByCategory : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	FString CategoryId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	uint8 bSearchByRarity : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	FString RarityId;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const FInventoryItem&, AddedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, const FInventoryItem&, RemovedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemQuantityChanged, const FInventoryItem&, Item, int32, OldQuantity, int32, NewQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemDurabilityChanged, const FInventoryItem&, Item, float, OldDurability, float, NewDurability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationProcessed, const FInventoryNotificationData&, NotificationData);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaAdvancedInventoryInterface defines comprehensive inventory management operations.
 * Inventory interfaces provide item addition, removal, searching, quantity management, durability handling,
 * and notification processing with event broadcasting for complete inventory system functionality.
 *
 * @see [Inventory Interface](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/InventorySystem)
 * @see UMounteaInventoryComponent
 * @see FInventoryItem
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
	* @return True if item was added successfully
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool AddItem(const FInventoryItem& Item);
	virtual bool AddItem_Implementation(const FInventoryItem& Item) = 0;

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
	* Adds an item from template
	* @param Template The template to create item from
	* @param Quantity Amount to add
	* @param Durability Initial durability
	* @return True if item was added successfully
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool AddItemFromTemplate(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1, const float Durability = 1.f);
	virtual bool AddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1, const float Durability = 1.f) = 0;

	/**
	* Removes items by template
	* @param Template Template to match
	* @param Quantity Amount to remove
	* @return True if items were removed
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool RemoveItemFromTemplate(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1);
	virtual bool RemoveItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) = 0;

	/**
	* Checks if can add item from template
	* @param Template Template to check
	* @param Quantity Amount to validate
	* @return True if item can be added
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool CanAddItemFromTemplate(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) const;
	virtual bool CanAddItemFromTemplate_Implementation(UMounteaInventoryItemTemplate* Template, const int32 Quantity = 1) const = 0;

	/**
	* Finds item based on Search conditions.
	* @param SearchParams Search params to search by.
	* @return Found item or nullptr
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Search")
	FInventoryItem FindItem(const FInventoryItemSearchParams& SearchParams) const;
	virtual FInventoryItem FindItem_Implementation(const FInventoryItemSearchParams& SearchParams) const = 0;

	/**
	* Finds item index based on Search conditions.
	* @param SearchParams Search params to search by.
	* @return Found item index or -1
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Search")
	int32 FindItemIndex(const FInventoryItemSearchParams& SearchParams) const;
	virtual int32 FindItemIndex_Implementation(const FInventoryItemSearchParams& SearchParams) const = 0;

	/**
	* Finds items based on Search conditions.
	* @param SearchParams Search params to search by.
	* @return Found items or empty array
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Search")
	TArray<FInventoryItem> FindItems(const FInventoryItemSearchParams& SearchParams) const;
	virtual TArray<FInventoryItem> FindItems_Implementation(const FInventoryItemSearchParams& SearchParams) const = 0;

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

	/**
	 * Checks if an item matching the provided search parameters exists in the inventory.
	 *
	 * @param SearchParams The search parameters used to locate the item.
	 * @return True if an item matching the search parameters exists in the inventory, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|Management")
	bool HasItem(const FInventoryItemSearchParams& SearchParams) const;
	virtual bool HasItem_Implementation(const FInventoryItemSearchParams& SearchParams) const = 0;

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

	// --- Events	------------------------------
	virtual FOnItemAdded& GetOnItemAddedEventHandle() = 0;
	virtual FOnItemRemoved& GetOnItemRemovedEventHandle() = 0;
	virtual FOnItemQuantityChanged& GetOnItemQuantityChangedEventHandle() = 0;
	virtual FOnItemDurabilityChanged& GetOnItemDurabilityChangedEventHandle() = 0;
	virtual FOnNotificationProcessed& GetOnNotificationProcessedEventHandle() = 0;
};
