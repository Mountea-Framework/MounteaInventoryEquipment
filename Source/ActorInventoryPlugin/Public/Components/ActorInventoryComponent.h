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
 * Inventory Component inherits from IActorInventoryInterface to avoid direct code references.
 *
 * Networking is not implemented.
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Component
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

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual UActorInventoryComponent* GetOtherInventory() const override;
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void SetOtherInventory(UActorInventoryComponent* NewOtherInventory) override;

	virtual bool FindItemByClass(const TSubclassOf<UInventoryItem> ItemClass) const override;
	virtual bool FindItemByGUID(const FGuid& Guid) const override;
	virtual bool FindItemByData(const FInventoryItemData& Data) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual bool IsItemInInventory(UInventoryItem* Item) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void CloseInventory() override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual UInventoryTypeDefinition* GetInventoryType() const override;

	
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
	virtual  UInventoryItem* GetItemByCategory(const FGuid& CategoryGuid) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual TArray<UInventoryItem*> GetItemsByCategory(const FGuid& CategoryGuid) const override;

	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void LoadInventoryContent(const class UDataTable* SourceTable) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual float GetInventoryWeight() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual void SetInventoryWeightLimit(const float& NewValue) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual bool UpdateInventoryWeight(const float& UpdateValue) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual float GetInventoryMaxWeight() const override;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual bool TransferItem(TScriptInterface<IActorInventoryInterface> SourceInventory, TScriptInterface<IActorInventoryInterface> TargetInventory, UInventoryItem* Item) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual TSubclassOf<UInventoryTransaction> GetMoveFromTransaction(const TScriptInterface<IActorInventoryInterface> Context) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual TSubclassOf<UInventoryTransaction> GetMoveToTransaction(const TScriptInterface<IActorInventoryInterface> Context) const override;
	
	virtual FOnInventoryUpdated& GetUpdateEventHandle() override;
	virtual FOnInventoryUpdateRequestProcessed& GetInventoryRequestProcessedHandle () override;
	virtual FOnInventoryLayoutSaveRequested& GetInventoryLayoutUpdateRequestHandle() override;
	virtual FOnItemActionRequested& GetItemActionRequestedHandle() override;
	virtual FOnInventoryOpenRequested& GetInventoryOpenRequestedHandle() override;

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

// Calling Broadcast from BP when Layout Save is requested
#pragma region Layout

	/**
	 * @brief This function should be called whenever you want to updated Saved Inventory Layout.
	 * @param Slot 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory|UI", meta=(DisplayName="Save To Inventory Layout"))
	void SaveToInventoryLayoutBP(const FInventorySlotData& Slot);
	
	/**
	 * @brief 
	 * @param Slot 
	 */
	virtual void SaveToInventoryLayout(const FInventorySlotData& Slot) override;

#pragma endregion Layout

// Calling Broadcast from BP when ActionKey is requested
#pragma region KeyActionsMapping

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory|UI", meta=(DisplayName="Process Item Key Action"))
	void ExecuteItemKeyActionBP(UInventoryItemSlot* ForItem, const FGuid& ActionGuid);

	UFUNCTION()
	virtual void ExecuteItemKeyAction(UInventoryItemSlot* ForItem, const FGuid& ActionGuid) override;

#pragma endregion KeyActionsMapping

#pragma region Events

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryUpdateRequestProcessed OnInventoryUpdateRequestProcessed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryLayoutSaveRequested OnInventoryLayoutSaveRequested;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnKeyPressed OnKeyPressed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnKeyReleased OnKeyReleased;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnItemActionRequested OnItemActionRequested;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryOpenRequested OnInventoryOpenRequested;

#pragma endregion Events

#pragma endregion InventoryInterface

	UFUNCTION(BlueprintCallable, Category="Inventory|UI")
	void SetInventoryLayout(const FInventoryLayout& InInventoryLayout);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|UI")
	FORCEINLINE FInventoryLayout GetInventoryLayout() const;

protected:

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Type", meta=(NoResetToDefault=true, BlueprintBaseOnly=true))
	UInventoryTypeDefinition* InventoryType = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|UI")
	UInventoryWidget* InventoryWidget = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|UI")
	UInventoryNotificationContainer* InventoryNotificationContainer = nullptr;
	
	/**
	 * List of Items that are currently in Inventory.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory")
	TArray<UInventoryItem*> InventoryItems;

	// Overrides default layout set in Inventory Manager
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|UI")
	uint8 bOverrideDefaultLayout : 1;

	// Custom Inventory Layout
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|UI", meta=(ShowOnlyInnerProperties=true, EditCondition="bOverrideDefaultLayout"))
	FInventoryLayout InventoryLayout;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Inventory|UI")
	float MaximumInventoryWeight = 50.f;
	
private:

	// Could be Loot Source, Shop or others
	UPROPERTY()
	UActorInventoryComponent* OtherInventory = nullptr;

	UPROPERTY()
	class UActorInventoryManagerComponent* InventoryManager = nullptr;

private:

	UFUNCTION()
	void UpdateInventory();
	
protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
