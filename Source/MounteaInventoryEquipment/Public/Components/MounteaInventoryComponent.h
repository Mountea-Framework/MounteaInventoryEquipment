// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "MounteaInventoryComponent.generated.h"

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

public:

	virtual TSubclassOf<UUserWidget> GetInventoryWBPClass_Implementation() override;
	virtual UUserWidget* GetInventoryWBP_Implementation() override;

	virtual bool LoadInventoryFromDataTable_Implementation(const UDataTable* SourceTable) override;
	virtual void SaveInventory_Implementation() override;
	
	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	virtual UMounteaInventoryItemBase* FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) const override;
	virtual TArray<UMounteaInventoryItemBase*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) const override;
	
	virtual bool AddOrUpdateItem_Implementation(UMounteaInventoryItemBase* NewItem, const int32 OptionalQuantity = 0) override;
	virtual bool AddItems_Implementation(TMap<UMounteaInventoryItemBase*,int32>& NewItems) override;
	virtual bool AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItemBase> ItemClass, const int32 OptionalQuantity = 0) override;
	virtual bool AddItemsFromClass_Implementation(TMap<TSubclassOf<UMounteaInventoryItemBase>, int32>& NewItemsClasses) override;

	virtual bool RemoveItem_Implementation(UMounteaInventoryItemBase* AffectedItem) override;
	virtual bool RemoveItems_Implementation(TArray<UMounteaInventoryItemBase*>& AffectedItems) override;

	virtual void RequestNetworkRefresh_Implementation() override;

	virtual AActor* GetOwningActor_Implementation() const override;

	virtual void SetInventoryWBPClass_Implementation(TSubclassOf<UUserWidget> NewInventoryWBPClass) override;
	virtual void SetInventoryWBP_Implementation(UUserWidget* NewWBP) override;
	
public:

	virtual FOnInventoryUpdated& GetInventoryUpdatedHandle() override
	{ return OnInventoryUpdated; };
	virtual FOnItemUpdated& GetItemAddedHandle() override
	{ return OnItemAdded; };
	virtual FOnItemUpdated& GetItemRemovedHandle() override
	{ return OnItemRemoved; };
	virtual FOnItemUpdated& GetItemUpdatedHandle() override
	{ return OnItemUpdated; };

private:

	UFUNCTION()
	void OnRep_Items();

	UFUNCTION(Client, Reliable)
	void ClientRefreshInventory();

protected:

	virtual bool TryAddItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity = 0);
	
	virtual bool TryRemoveItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity = 0);
	
	virtual bool TryAddItem_NewItem(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity = 0);
	virtual bool TryAddItem_UpdateExisting(UMounteaInventoryItemBase* Existing, UMounteaInventoryItemBase* NewItem, const int32 OptionalQuantity = 0);
	
	UFUNCTION(Server, Unreliable)
	void PostInventoryUpdated(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostInventoryUpdated_Client(const FInventoryUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemAdded(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemAdded_Client(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemRemoved(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemRemoved_Client(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION(Server, Unreliable)
	void PostItemUpdated(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION(Client, Unreliable)
	void PostItemUpdated_Client(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);

private:

	int32 CalculateMaxPossibleQuantity(const UMounteaInventoryItemBase* Item, const UMounteaInventoryItemBase* OtherItem = nullptr, int32 OptionalQuantity = 0) const;

	/**
	 * Do not call directly!
	 * Use `AddItem` interface function instead.
	 *
	 * To use Interface call request `Execute_AddItem({interface}, {item})`
	 */
	virtual bool AddItem_Internal(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity = 0);
	virtual bool UpdateItem_Internal(UMounteaInventoryItemBase* Item, const int32 OptionalQuantity = 0);
	virtual bool  RemoveItem_Internal(UMounteaInventoryItemBase* Item, int32 OptionalQuantity = 0);
	
	UFUNCTION()
	void PostInventoryUpdated_Client_RequestUpdate(const FInventoryUpdateResult& UpdateContext);
	bool CanExecuteCosmetics() const;

	UFUNCTION()
	void PostItemAdded_Client_RequestUpdate(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemRemoved_Client_RequestUpdate(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	UFUNCTION()
	void PostItemUpdated_Client_RequestUpdate(UMounteaInventoryItemBase* Item, const FItemUpdateResult& UpdateContext);
	
	void RequestInventoryNotification(const FInventoryUpdateResult& UpdateContext) const;
	void RequestItemNotification(const FItemUpdateResult& UpdateContext) const;
	
#pragma endregion

#pragma region VARIABLES
	
protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory" )
	FOnInventoryUpdated OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory" )
	FOnInventoryUpdated OnInventoryUpdated_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemAdded;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemAdded_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemRemoved;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemRemoved_Client;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemUpdated_Client;

protected:

	UPROPERTY(SaveGame, EditAnywhere, Category="1. Required", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface"))
	TSubclassOf<UUserWidget> InventoryWBPClass;

	UPROPERTY(Transient, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	UUserWidget* InventoryWBP = nullptr;

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<UMounteaInventoryItemBase*> Items;

	UPROPERTY()
	TArray<UMounteaInventoryItemBase*> ClientLastReceivedItems;

private:

	UPROPERTY()
	int32 ReplicatedItemsKey;
	
	UPROPERTY()
	FTimerHandle TimerHandle_RequestInventorySyncTimerHandle;
	UPROPERTY()
	FTimerHandle TimerHandle_RequestItemSyncTimerHandle;
	UPROPERTY(EditAnywhere, Category="2. Debug")
	float Duration_RequestSyncTimerHandle = 0.2f;

	TArray<UMounteaInventoryItemBase*> RemovedItems;

#pragma endregion
	
};

#undef LOCTEXT_NAMESPACE