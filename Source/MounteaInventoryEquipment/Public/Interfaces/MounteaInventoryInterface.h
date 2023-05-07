// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryInterface.generated.h"

class UMounteaInventoryItemBase;

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMounteaInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUpdated, class UMounteaInventoryItemBase*, Item, const FString&, UpdateMessage);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TSubclassOf<UUserWidget> GetInventoryWBPClass();
	virtual TSubclassOf<UUserWidget> GetInventoryWBPClass_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UUserWidget* GetInventoryWBP();
	virtual UUserWidget* GetInventoryWBP_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool LoadInventoryFromDataTable(const class UDataTable* SourceTable);
	virtual bool LoadInventoryFromDataTable_Implementation(const class UDataTable* SourceTable) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SaveInventory();
	virtual void SaveInventory_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaInventoryItemBase* FindItem(const FItemRetrievalFilter& SearchFilter);
	virtual UMounteaInventoryItemBase* FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) = 0;

	/***
	 * ❗  Only checks for first Item that matches Search Filter.
	 *
	 * Takes a single argument of type FItemRetrievalFilter, which allows for searching the inventory for an item based on various parameters such as item ID, item class, tags, or GUID.
	 * When called, HasItem will check the inventory for the existence of an item that matches the provided search filter. If such an item exists, the function will return true; otherwise, it will return false.
	 * This function can be useful for quickly checking whether a particular item is present in the inventory before attempting to perform further operations with it, such as removing or using it.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasItem(const FItemRetrievalFilter& SearchFilter);
	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInventoryItemBase*> GetItems(const FItemRetrievalFilter OptionalFilter);
	virtual TArray<UMounteaInventoryItemBase*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItem(UMounteaInventoryItemBase* NewItem);
	virtual bool AddItem_Implementation(UMounteaInventoryItemBase* NewItem) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItemFromClass(TSubclassOf<UMounteaInventoryItemBase> ItemClass);
	virtual bool AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItems(TArray<UMounteaInventoryItemBase*>& NewItems);
	virtual bool AddItems_Implementation(TArray<UMounteaInventoryItemBase*>& NewItems) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItemsFromClass(TArray<TSubclassOf<UMounteaInventoryItemBase>>& NewItemsClasses);
	virtual bool AddItemsFromClass_Implementation(TArray<TSubclassOf<UMounteaInventoryItemBase>>& NewItemsClasses) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool RemoveItem(UMounteaInventoryItemBase* AffectedItem);
	virtual bool RemoveItem_Implementation(UMounteaInventoryItemBase* AffectedItem) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool RemoveItems(TArray<UMounteaInventoryItemBase*>& AffectedItems);
	virtual bool RemoveItems_Implementation(TArray<UMounteaInventoryItemBase*>& AffectedItems) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void RequestNetworkRefresh();
	virtual void RequestNetworkRefresh_Implementation() = 0;
	
public:

	virtual FOnInventoryUpdated& GetInventoryUpdatedHandle() = 0;
	virtual FOnItemUpdated& GetItemAddedHandle() = 0;
	virtual FOnItemUpdated& GetItemRemovedHandle() = 0;
	virtual FOnItemUpdated& GetItemUpdatedHandle() = 0;
};
