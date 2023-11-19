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
	
	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	
	virtual FInventoryUpdateResult AddItemToInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) override;
	virtual TArray<FInventoryUpdateResult> AddItemsToInventory_Implementation(const TMap<UMounteaInstancedItem*,int32>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType)) int32 TransactionTypeFlags = 10 ) override;
	virtual FInventoryUpdateResult RemoveItemFromInventory_Implementation(UMounteaInstancedItem* Item) override;
	virtual TArray<FInventoryUpdateResult> RemoveItemsFromInventory_Implementation(const TArray<UMounteaInstancedItem*>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags = 10) override;
	virtual FInventoryUpdateResult ReduceItemInInventory_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) override;
	virtual TArray<FInventoryUpdateResult> ReduceItemsInInventory_Implementation(const TMap<UMounteaInstancedItem*, int32>& Items, UPARAM(meta = (Bitmask, BitmaskEnum = EInventoryTransactionType))  int32 TransactionTypeFlags = 10) override;
	
	virtual bool CanAddItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity) const override;
	virtual bool CanRemoveItem_Implementation(UMounteaInstancedItem* Item) const override;
	
	virtual UMounteaInstancedItem* SearchSingleItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	virtual TArray<UMounteaInstancedItem*> SearchMultipleItems_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	virtual TArray<UMounteaInstancedItem*> GetItems_Implementation() const override;
	
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

	UFUNCTION(Server, Unreliable)
	void PostInventoryUpdated(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemAdded(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemRemoved(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemUpdated(const FInventoryUpdateResult& UpdateContext);

	void RequestInventoryNotification(const FInventoryUpdateResult& UpdateContext) const;

	virtual FInventoryUpdateResult ProcessItemAction_Implementation(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context) override;
	
protected:

	/**
	 * Checks if the owning actor has a network role of either Authority or Autonomous Proxy.
	 * The function is used to determine if the actor is in a suitable state to perform certain operations,
	 * particularly those that are visual or UI-related and may not be relevant or appropriate for actors in other network roles.
	 * 
	 * @return True if the owning actor is either Authority or Autonomous Proxy, false otherwise.
	 */
	bool IsAuthorityOrAutonomousProxy() const;
	
#if WITH_EDITOR
private:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
  
#endif

private:

	UFUNCTION()
	void OnRep_Items();

protected:
	
	UFUNCTION(Client, Unreliable)
	void PostInventoryUpdated_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemAdded_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemRemoved_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemUpdated_Client(const FInventoryUpdateResult& UpdateContext);

	virtual void RequestNetworkRefresh_Implementation() override;
	
private:
	
	UFUNCTION()
	void PostInventoryUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);

	UFUNCTION()
	void PostItemAdded_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemRemoved_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	
	UFUNCTION(Server, Unreliable)
	void RequestNetworkRefresh_Server();

	UFUNCTION(Server, Reliable)
	void ProcessItemAction_Server(UMounteaInventoryItemAction* Action, UMounteaInstancedItem* Item, FMounteaDynamicDelegateContext Context);
	
/*===============================================================================
		IN PROGRESS
		
		Following functions are already being updated.
===============================================================================*/

	
/*===============================================================================
		SUBJECT OF CHANGE
		
		Following functions are using outdated, wrong class definitions and functions.
===============================================================================*/

	virtual bool LoadInventoryFromDataTable_Implementation(const UMounteaInventoryItemsTable* SourceTable) override;
	virtual void SaveInventory_Implementation() override;

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
	
	UPROPERTY(VisibleAnywhere, Category="2. Debug")
	int32 ReplicatedItemsKey = 0;
	
	// Filled from RemoveFromItem to keep track of Items that were removed
	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<FItemSlot> ModifiedSlots;

	UPROPERTY()
	FTimerHandle TimerHandle_RequestInventorySyncTimerHandle;
	
	UPROPERTY(EditAnywhere, Category="2. Debug")
	float Duration_RequestSyncTimerHandle = 0.1f;

#pragma endregion
	
};

#undef LOCTEXT_NAMESPACE