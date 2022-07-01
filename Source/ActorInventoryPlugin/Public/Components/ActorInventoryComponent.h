// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Helpers/InventoryHelpers.h"
#include "Interfaces/ActorInventoryInterface.h"

#include "ActorInventoryComponent.generated.h"

class UInventoryItem;
class UInventoryNotificationContainer;

/**
 * Implement an Actor component for inventory.
 *
 * An Inventory Component is a non-transient component that enables its owning Actor to manage Inventory Items.
 *
 * @warning Networking is not implemented.
 *
 * @see https://sites.google.com/view/dominikpavlicek/home/documentation
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Component", ShortTooltip="Implement an Actor component for inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryComponent : public UActorComponent, public IActorInventoryInterface
{
	GENERATED_BODY()

public:	

	UActorInventoryComponent();

#pragma region InventoryInterface

public:

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual bool AddItemToInventory(UInventoryItem* Item) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual bool AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual void RemoveItemsFromInventory(const TArray<UInventoryItem*>& ListOfItems) override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void RemoveItemFromInventory(UInventoryItem* Item) override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void SubtractItemFromInventory(UInventoryItem* Item, int32 Amount) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual void SubtractItemsFromInventory(const TMap<UInventoryItem*, int32>& ListOfItems) override;


	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void SetInventoryItems(const TArray<UInventoryItem*> Items) override {InventoryItems = Items; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual TArray<UInventoryItem*> GetInventoryItems() const override {return InventoryItems; };
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual int32 GetItemQuantity(UInventoryItem* Item) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual void GetInventorySlotsData(TArray<FInventorySlotData>& SlotData) override;
	

	virtual bool FindItemByClass(const TSubclassOf<UInventoryItem> ItemClass) const override;
	virtual bool FindItemByGUID(const FGuid& Guid) const override;
	virtual bool FindItemByData(const FInventoryItemData& Data) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual bool IsItemInInventory(UInventoryItem* Item) const override;

	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual UInventoryItem* GetItemByData(const FInventoryItemData& ItemData) const override;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual TArray<UInventoryItem*> GetItemsByData(const FInventoryItemData& ItemData) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual UInventoryItem* GetItemByGUID(const FGuid& Guid) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual TArray<UInventoryItem*> GetItemsByGUID(const FGuid& Guid) const override;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual UInventoryItem* GetItemByClass(const TSubclassOf<UInventoryItem>& Class) const override;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual TArray<UInventoryItem*> GetItemsByClass(const TSubclassOf<UInventoryItem>& Class) const override;

	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void LoadInventoryContent(const class UDataTable* SourceTable) override;

	
	virtual FOnInventoryUpdated& GetUpdateEventHandle() override;
	virtual FOnInventoryUpdateRequestProcessed& GetInventoryRequestProcessedHandle () override;
	virtual FOnInventoryLayoutSaveRequested& GetInventoryLayoutUpdateRequestHandle() override;

#pragma region InventoryWidget

public:
	
	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	virtual void SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	virtual UInventoryWidget* GetInventoryWidgetPtr() const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE UInventoryNotificationContainer* GetNotificationContainerPtr() const
	{
		return InventoryNotificationContainer;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetNotificationContainerPtr(UInventoryNotificationContainer* Widget);

#pragma endregion InventoryWidget

#pragma region Events

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryUpdateRequestProcessed OnInventoryUpdateRequestProcessed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryLayoutSaveRequested OnInventoryLayoutSaveRequested;

#pragma endregion Events

#pragma endregion InventoryInterface

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryLayout(const FInventoryLayout& InInventoryLayout);

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SaveToInventoryLayout(FIntPoint& SlotCoordinates, FInventorySlotData& Slot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE FInventoryLayout GetInventoryLayout() const
	{
		return InventoryLayout;
	}
	
protected:

	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|UI")
	UInventoryWidget* InventoryWidget = nullptr;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|UI")
	UInventoryNotificationContainer* InventoryNotificationContainer = nullptr;
	
	/**
	 * List of Items that are currently in Inventory.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TArray<UInventoryItem*> InventoryItems;

	UPROPERTY(EditDefaultsOnly, Category="Inventory|UI", meta=(ShowOnlyInnerProperties=true))
	FInventoryLayout InventoryLayout;

	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TArray<FInventorySlotData> InventorySlotsData;
	
private:

	UPROPERTY()
	class UActorInventoryManagerComponent* InventoryManager = nullptr;
};
