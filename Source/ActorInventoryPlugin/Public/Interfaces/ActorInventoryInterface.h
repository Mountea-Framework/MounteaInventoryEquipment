// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItem.h"
#include "UObject/Interface.h"
#include "ActorInventoryInterface.generated.h"

enum class EInventoryContext : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, UActorComponent*, InventoryComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdateRequestProcessed, EInventoryContext, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryLayoutSaveRequested, const FIntPoint&, SlotCoordinates, class UInventoryItemSlot*, Slot);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UActorInventoryInterface : public UInterface
{
	GENERATED_BODY()
	
};

class UInventoryItem;
class UInventoryWidget;

/**
 * 
 */
class ACTORINVENTORYPLUGIN_API IActorInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	
	virtual bool AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems) = 0;
	virtual bool AddItemToInventory(UInventoryItem* Item) = 0;

	virtual void RemoveItemsFromInventory(const TArray<UInventoryItem*>& ListOfItems) = 0;
	virtual void RemoveItemFromInventory(UInventoryItem* Item) = 0;

	virtual void SubtractItemsFromInventory(const TMap<UInventoryItem*, int32>& ListOfItems) = 0;
	virtual void SubtractItemFromInventory(UInventoryItem* Item, const int32 Amount) = 0;
	
	virtual void SetInventoryItems(const TArray<UInventoryItem*> Items) = 0;
	
	virtual TArray<UInventoryItem*> GetInventoryItems() const = 0;
	virtual int32 GetItemQuantity(UInventoryItem* Item) const = 0;

	virtual void GetInventorySlotsData(TArray<FInventorySlotData>& SlotData) = 0;

	virtual bool FindItemByClass(const TSubclassOf<UInventoryItem> ItemClass) const = 0;
	virtual bool FindItemByGUID(const FGuid& Guid) const = 0;
	virtual bool FindItemByData(const FInventoryItemData& Data) const = 0;
	virtual bool IsItemInInventory(UInventoryItem* Item) const = 0;

	virtual UInventoryItem* GetItemByData(const FInventoryItemData& ItemData) const = 0;
	virtual TArray<UInventoryItem*> GetItemsByData(const FInventoryItemData& ItemData) const = 0;
	virtual UInventoryItem* GetItemByGUID(const FGuid& Guid) const = 0;
	virtual TArray<UInventoryItem*> GetItemsByGUID(const FGuid& Guid) const = 0;
	virtual UInventoryItem* GetItemByClass(const TSubclassOf<UInventoryItem>& Class) const = 0;
	virtual TArray<UInventoryItem*> GetItemsByClass(const TSubclassOf<UInventoryItem>& Class) const = 0;
	
	virtual void LoadInventoryContent(const class UDataTable* SourceTable) = 0;

	
	virtual void SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget) = 0;
	virtual UInventoryWidget* GetInventoryWidgetPtr() const = 0;

	virtual FOnInventoryUpdated& GetUpdateEventHandle() = 0;
	virtual FOnInventoryUpdateRequestProcessed& GetInventoryRequestProcessedHandle () = 0;
	virtual FOnInventoryLayoutSaveRequested& GetInventoryLayoutUpdateRequestHandle() = 0;

};