// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInventoryInterface.generated.h"

enum class EInventoryContext : uint8;
class UActorInventoryComponent;
class UInventoryItem;
class UInventoryItemSlot;

struct FInventorySlotData;
struct FInventoryItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdateRequestProcessed, EInventoryContext, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryLayoutSaveRequested, const FInventorySlotData&, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyPressed, const FKey&, PressedKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyReleased, const FKey&, ReleasedKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemActionRequested, UInventoryItemSlot*, InventoryItem, const FGuid&, ActionGuid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryOpenRequested);

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UActorInventoryInterface : public UInterface
{
	GENERATED_BODY()
	
};

class UInventoryItem;
class UInventoryWidget;
class UInventoryItemSlot;
class UInventoryTypeDefinition;
class UInventoryTransaction;

/**
 * Actor Inventory Interface.
 *
 * Interface which represents the Inventory which can be attached to any Actor.
 * For Player, this Inventory must be Attached to PlayerController!
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Actor-Inventory-Interface
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

	virtual UActorInventoryComponent* GetOtherInventory() const = 0;
	virtual void SetOtherInventory(UActorInventoryComponent* NewOtherInventory) = 0;

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
	virtual UInventoryItem* GetItemByCategory(const FGuid& CategoryGuid) const = 0;
	virtual TArray<UInventoryItem*> GetItemsByCategory(const FGuid& CategoryGuid) const = 0;
	
	virtual void LoadInventoryContent(const class UDataTable* SourceTable) = 0;
	virtual void SaveToInventoryLayout(const FInventorySlotData& Slot) = 0;

	virtual UInventoryTypeDefinition* GetInventoryType() const = 0;
	
	virtual void SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget) = 0;
	virtual UInventoryWidget* GetInventoryWidgetPtr() const = 0;

	virtual float GetInventoryWeight() const = 0;
	virtual void SetInventoryWeightLimit(const float& NewValue) = 0;
	virtual bool UpdateInventoryWeight(const float& UpdateValue) = 0;
	virtual float GetInventoryMaxWeight() const = 0;

	virtual void ExecuteItemKeyAction(UInventoryItemSlot* ForItem, const FGuid& ActionGuid) = 0;
		
	virtual TSubclassOf<UInventoryTransaction> GetMoveFromTransaction(const TScriptInterface<IActorInventoryInterface> Context) const = 0;
	virtual TSubclassOf<UInventoryTransaction> GetMoveToTransaction(const TScriptInterface<IActorInventoryInterface> Context) const = 0;
	
	virtual FOnInventoryUpdated& GetUpdateEventHandle() = 0;
	virtual FOnInventoryUpdateRequestProcessed& GetInventoryRequestProcessedHandle () = 0;
	virtual FOnInventoryLayoutSaveRequested& GetInventoryLayoutUpdateRequestHandle() = 0;
	virtual FOnItemActionRequested& GetItemActionRequestedHandle() = 0;
	virtual FOnInventoryOpenRequested& GetInventoryOpenRequestedHandle() = 0;

	virtual void CloseInventory() = 0;

	virtual bool TransferItem(TScriptInterface<IActorInventoryInterface> SourceInventory, TScriptInterface<IActorInventoryInterface> TargetInventory, UInventoryItem* Item) = 0;
};