// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryInterface.generated.h"

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TSubclassOf<UMounteaBaseUserWidget> GetInventoryUIClass() const;
	virtual TSubclassOf<UMounteaBaseUserWidget> GetInventoryUIClass_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetInventoryUIClass(TSubclassOf<UMounteaBaseUserWidget> NewInventoryUIClass);
	virtual void SetInventoryUIClass_Implementation(TSubclassOf<UMounteaBaseUserWidget> NewInventoryUIClass) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetInventoryUI(UMounteaBaseUserWidget* NewUI);
	virtual void SetInventoryUI_Implementation(UMounteaBaseUserWidget* NewUI) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaBaseUserWidget* GetInventoryUI() const;
	virtual UMounteaBaseUserWidget* GetInventoryUI_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool LoadInventoryFromDataTable(const class UMounteaInventoryItemsTable* SourceTable);
	virtual bool LoadInventoryFromDataTable_Implementation(const class UMounteaInventoryItemsTable* SourceTable) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SaveInventory();
	virtual void SaveInventory_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult AddItemToInventory(UMounteaInstancedItem* Item, const int32& Quantity = 1);
	virtual FInventoryUpdateResult AddItemToInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult RemoveItemFromInventory(UMounteaInstancedItem* Item);
	virtual FInventoryUpdateResult RemoveItemFromInventory_Implementation(UMounteaInstancedItem* Item) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult ReduceItemInInventory(UMounteaInstancedItem* Item, const int32& Quantity = 1);
	virtual FInventoryUpdateResult ReduceItemInInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanAddItem(UMounteaInstancedItem* Item, const int32& Quantity) const;
	virtual bool CanAddItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanRemoveItem(UMounteaInstancedItem* Item) const;
	virtual bool CanRemoveItem_Implementation(UMounteaInstancedItem* Item) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaInstancedItem* SearchSingleItem(const FItemRetrievalFilter& SearchFilter) const;
	virtual UMounteaInstancedItem* SearchSingleItem_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInstancedItem*> SearchMultipleItems(const FItemRetrievalFilter& SearchFilter) const;
	virtual TArray<UMounteaInstancedItem*> SearchMultipleItems_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasItem(const FItemRetrievalFilter& SearchFilter) const;
	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool SetInventoryFlags(const FGameplayTagContainer& NewFlags);
	virtual bool SetInventoryFlags_Implementation(const FGameplayTagContainer& NewFlags) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool SetInventoryFlag(const FGameplayTag& NewFlag);
	virtual bool SetInventoryFlag_Implementation(const FGameplayTag& NewFlag) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FGameplayTagContainer GetInventoryFlags() const;
	virtual FGameplayTagContainer GetInventoryFlags_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasFlag(const FGameplayTag& SearchedFlag, const bool bSearchExact = true);
	virtual bool HasFlag_Implementation(const FGameplayTag& SearchedFlag, const bool bSearchExact = true) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool HasFlags(const FGameplayTagContainer& SearchedFlags, const bool bSearchExact = true);
	virtual bool HasFlags_Implementation(const FGameplayTagContainer& SearchedFlags, const bool bSearchExact = true, const bool bSearchFast = true) = 0;

	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaInventoryItemBase* FindItem(const FItemRetrievalFilter& SearchFilter) const;
	virtual UMounteaInventoryItemBase* FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) const = 0;

	

	/**
	 * Returns an array of pointers to UMounteaInventoryItemBase objects that match the specified filter criteria.
	 * The filter options are defined in the FItemRetrievalFilter structure, which contains fields to search by item, class, tag, or GUID, or a additive combination of each.
	 * By default, if no filter is specified, the function returns all items in the inventory.
	 *
	 * @param OptionalFilter An optional item retrieval filter specifying which items to retrieve.
	 *
	 * @return An array of inventory items that meet the specified filter criteria. If no filter is provided, returns all items.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInventoryItemBase*> GetItems(const FItemRetrievalFilter OptionalFilter) const;
	virtual TArray<UMounteaInventoryItemBase*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddOrUpdateItem(UMounteaInventoryItemBase* NewItem, const int32& Quantity = 1);
	virtual bool AddOrUpdateItem_Implementation(UMounteaInventoryItemBase* NewItem, const int32& Quantity = 1) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItemFromClass(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32& Quantity = 1);
	virtual bool AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32& Quantity = 1) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItems(TMap<UMounteaInventoryItemBase*,int32>& NewItems);
	virtual bool AddItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& NewItems) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItemsFromClass(TMap<TSubclassOf<UMounteaInventoryItemBase>, int32>& NewItemsClasses);
	virtual bool AddItemsFromClass_Implementation(TMap<TSubclassOf<UMounteaInventoryItemBase>, int32>& NewItemsClasses) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool RemoveItem(UMounteaInventoryItemBase* AffectedItem, const int32& Amount = 1);
	virtual bool RemoveItem_Implementation(UMounteaInventoryItemBase* AffectedItem, const int32& Amount = 1) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool RemoveItems(TMap<UMounteaInventoryItemBase*,int32>& AffectedItems);
	virtual bool RemoveItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& AffectedItems) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void RequestNetworkRefresh();
	virtual void RequestNetworkRefresh_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void ProcessItemAction(UMounteaInventoryItemAction* Action, UMounteaInventoryItemBase* Item, FMounteaDynamicDelegateContext Context);
	virtual void ProcessItemAction_Implementation(UMounteaInventoryItemAction* Action, UMounteaInventoryItemBase* Item, FMounteaDynamicDelegateContext Context) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	UMounteaInventoryConfig* GetInventoryConfig( TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult) const;
	virtual UMounteaInventoryConfig* GetInventoryConfig_Implementation( TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TSubclassOf<UMounteaInventoryConfig> GetInventoryConfigClass() const;
	virtual TSubclassOf<UMounteaInventoryConfig> GetInventoryConfigClass_Implementation() const = 0;
	
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool DoesHaveAuthority() const;
	virtual bool DoesHaveAuthority_Implementation() const = 0;
	
public:

	virtual FOnInventoryUpdated& GetInventoryUpdatedHandle() = 0;
	virtual FOnInventoryUpdated& GetItemAddedHandle() = 0;
	virtual FOnInventoryUpdated& GetItemRemovedHandle() = 0;
	virtual FOnInventoryUpdated& GetItemUpdatedHandle() = 0;
};
