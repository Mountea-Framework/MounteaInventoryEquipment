// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Helpers/MounteaItemHelpers.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Setup/MounteaInventoryConfig.h"

#include "MounteaInventoryComponent.generated.h"

class UMounteaTransactionPayload;
class UMounteaBaseUserWidget;
class UMounteaInventoryItemBase;

#define LOCTEXT_NAMESPACE "MounteaInventoryComponent"

/**
 * Mountea Inventory Component.
 *
 * Implements `IMounteaInventoryInterface`.
 *
 * Provides inventory functionality to Actors that it is attached to.
 * It allows Actors to hold and manage collections of items, which can be added, removed, and manipulated in various ways.
 * The component provides a customizable inventory system that can be configured to fit the needs of different types of games, such as RPGs or survival games.
 * It also offers a range of features such as stacking, item properties, and events that can be used to create complex interactions between items and game mechanics.
 *
 * This system is designed to be modular and extensible, allowing developers to easily add new features or modify existing ones to suit their specific requirements.
 * If current implementation is not what you are looking for, the `IMounteaInventoryInterface` comes with all functions exposed, allowing recreating the whole system in Blueprints.
 *
 * @see UActorComponent
 * @see IMounteaInventoryInterface
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Interface
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Component
*/
UCLASS(ClassGroup=(Mountea), Blueprintable, hideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Mountea Inventory"))
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryComponent : public UActorComponent, public IMounteaInventoryInterface
{
	GENERATED_BODY()

public:
	
	UMounteaInventoryComponent();
	
#pragma region FUNCTIONS
protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void PostInitProperties() override;

public:

	virtual TSubclassOf<UMounteaBaseUserWidget> GetInventoryUIClass_Implementation() const override;
	virtual UMounteaBaseUserWidget* GetInventoryUI_Implementation() const override;

	virtual void SetInventoryUIClass_Implementation(TSubclassOf<UMounteaBaseUserWidget> NewInventoryWBPClass) override;
	virtual void SetInventoryUI_Implementation(UMounteaBaseUserWidget* NewWBP) override;

	virtual bool LoadInventoryFromDataTable_Implementation(const UMounteaInventoryItemsTable* SourceTable) override;
	virtual void SaveInventory_Implementation() override;
	
	/**
	 * Checks if the inventory contains an item that matches the given search filter.
	 * @param SearchFilter A given filter based on which the search engine will try to find the item. It is required to fill!
	 * @return True if item based on `SearchFilter` is found, false otherwise.
	 */
	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	
	virtual FInventoryUpdateResult AddItemToInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) override;
	virtual FInventoryUpdateResult RemoveItemFromInventory_Implementation(UMounteaInstancedItem* Item) override;
	virtual FInventoryUpdateResult ReduceItemInInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) override;
	
	virtual bool CanAddItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity) const override;
	virtual bool CanRemoveItem_Implementation(UMounteaInstancedItem* Item) const override;
	
	virtual UMounteaInstancedItem* SearchSingleItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	virtual TArray<UMounteaInstancedItem*> SearchMultipleItems_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	
	virtual FGameplayTagContainer GetInventoryFlags_Implementation() const override;
	virtual bool HasFlag_Implementation(const FGameplayTag& SearchedFlag, const bool bSearchExact = true) override;
	virtual bool HasFlags_Implementation(const FGameplayTagContainer& SearchedFlags, const bool bSearchExact = true, const bool bSearchFast = true) override;
	virtual bool SetInventoryFlags_Implementation(const FGameplayTagContainer& NewFlags) override;
	virtual bool SetInventoryFlag_Implementation(const FGameplayTag& NewFlag) override;

	virtual AActor* GetOwningActor_Implementation() const override;
	virtual bool DoesHaveAuthority_Implementation() const override;

	virtual UMounteaInventoryConfig* GetInventoryConfig_Implementation( TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult) const override;
	virtual TSubclassOf<UMounteaInventoryConfig> GetInventoryConfigClass_Implementation() const override;

public:

	virtual FOnInventoryUpdated& GetInventoryUpdatedHandle() override
	{ return OnInventoryUpdated; };
	virtual FOnInventoryUpdated& GetItemAddedHandle() override
	{ return OnItemAdded; };
	virtual FOnInventoryUpdated& GetItemRemovedHandle() override
	{ return OnItemRemoved; };
	virtual FOnInventoryUpdated& GetItemUpdatedHandle() override
	{ return OnItemUpdated; };

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void AddItemToInventory_Server(UMounteaInstancedItem* Item, const int32& Quantity = 1);
	UFUNCTION(Server, Reliable, WithValidation)
	void RemoveItemFromInventory_Server(UMounteaInstancedItem* Item);
	UFUNCTION(Server, Reliable, WithValidation)
	void ReduceItemInInventory_Server(UMounteaInstancedItem* Item, const int32& Quantity = 1);

protected:
	
	bool CanExecuteCosmetics() const;
	
/*===============================================================================
		IN PROGRESS
		
		Following functions are already being updated.
===============================================================================*/

protected:

	UFUNCTION(Server, Unreliable)
	void PostInventoryUpdated(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemAdded(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemRemoved(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemUpdated(const FInventoryUpdateResult& UpdateContext);
	
	UFUNCTION(Client, Unreliable)
	void PostInventoryUpdated_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemAdded_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemRemoved_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemUpdated_Client(const FInventoryUpdateResult& UpdateContext);

	void RequestInventoryNotification(const FInventoryUpdateResult& UpdateContext) const;
	void RequestItemNotification(const FInventoryUpdateResult& UpdateContext) const;

private:

	UFUNCTION()
	void OnRep_Items();

protected:

#if WITH_EDITOR
private:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
  
#endif
	
/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/
	
	virtual UMounteaInventoryItemBase* FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	virtual TArray<UMounteaInventoryItemBase*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) const override;
	
	virtual bool AddOrUpdateItem_Implementation(UMounteaInventoryItemBase* NewItem, const int32& Quantity = 1) override;
	virtual bool AddItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& NewItems) override;
	virtual bool AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32& Quantity = 1) override;
	virtual bool AddItemsFromClass_Implementation(TMap<TSubclassOf<UMounteaInventoryItemBase>, int32>& NewItemsClasses) override;

	virtual bool RemoveItem_Implementation(UMounteaInventoryItemBase* AffectedItem, const int32& Quantity = 1) override;
	virtual bool RemoveItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& AffectedItems) override;

	virtual void RequestNetworkRefresh_Implementation() override;
	
	virtual void ProcessItemAction_Implementation(UMounteaInventoryItemAction* Action, UMounteaInventoryItemBase* Item, FMounteaDynamicDelegateContext Context) override;

protected:
	
	UFUNCTION(Client, Reliable)
	void ClientRefreshInventory();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void TryAddItem_Server(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);
	bool TryAddItem(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);

	UFUNCTION(Server, Reliable, WithValidation)
	void TryRemoveItem_Server(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);
	bool TryRemoveItem(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);
	
	bool TryAddItem_NewItem(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);
	bool TryAddItem_UpdateExisting(UMounteaInventoryItemBase* Existing, UMounteaInventoryItemBase* NewItem, const int32 Quantity = 0);

private:

	/**
	 * Do not call directly!
	 * Use `AddItem` interface function instead.
	 *
	 * To use Interface call request `Execute_AddItem({interface}, {item})`
	 */
	virtual bool AddItem_Internal(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);
	virtual bool UpdateItem_Internal(UMounteaInventoryItemBase* Item, const int32 Quantity = 0);
	virtual bool  RemoveItem_Internal(UMounteaInventoryItemBase* Item, int32 Quantity = 0);
	
	UFUNCTION()
	void PostInventoryUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);

	UFUNCTION()
	void PostItemAdded_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemRemoved_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	
	bool HasItem_Simple(const FItemRetrievalFilter& SearchFilter) const;
	bool HasItem_Multithreading(const FItemRetrievalFilter& SearchFilter) const; 
	UMounteaInventoryItemBase* FindItem_Simple(const FItemRetrievalFilter& SearchFilter) const;
	UMounteaInventoryItemBase* FindItem_Multithreading(const FItemRetrievalFilter& SearchFilter) const;
	TArray<UMounteaInventoryItemBase*> GetItems_Simple(const FItemRetrievalFilter OptionalFilter) const;
	TArray<UMounteaInventoryItemBase*> GetItems_Multithreading(const FItemRetrievalFilter OptionalFilter) const;


#pragma endregion

#pragma region VARIABLES
	
protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory" )
	FOnInventoryUpdated OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory" )
	FOnInventoryUpdated OnInventoryUpdated_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnItemAdded;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnItemAdded_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnItemRemoved;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnItemRemoved_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnItemUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnItemUpdated_Client;

protected:

	UPROPERTY(SaveGame, EditAnywhere, Category="1. Required", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface"))
	TSubclassOf<UMounteaBaseUserWidget> InventoryWBPClass;

	UPROPERTY(Transient, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	UMounteaBaseUserWidget* InventoryWBP = nullptr;

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, DisallowCreateNew, NoElementDuplicate))
	TArray<FItemSlot> InventorySlots;

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, DisallowCreateNew, NoElementDuplicate))
	FGameplayTagContainer InventoryFlags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "4. Config", NoClear, meta=(NoResetToDefault))
	FMounteaInventoryConfigBase InventoryConfig;

private:
	
	UPROPERTY()
	int32 ReplicatedItemsKey = 0;
	
	// Filled from RemoveFromItem to keep track of Items that were removed
	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<FItemSlot> ModifiedSlots;
	
/*===============================================================================
			SUBJECT OF CHANGE
			
			Following variable are using outdated, wrong class definitions and functions.
===============================================================================*/

protected:
	
	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<UMounteaInventoryItemBase*> Items;

private:
	
	UPROPERTY()
	FTimerHandle TimerHandle_RequestInventorySyncTimerHandle;
	UPROPERTY()
	FTimerHandle TimerHandle_RequestItemSyncTimerHandle;
	UPROPERTY(EditAnywhere, Category="2. Debug")
	float Duration_RequestSyncTimerHandle = 0.2f;
		
	TArray<UMounteaInventoryItemBase*> RemovedItems;

	//TODO: Settings?
	const int32 ChunkSize = 100;

#pragma endregion
	
};

#undef LOCTEXT_NAMESPACE