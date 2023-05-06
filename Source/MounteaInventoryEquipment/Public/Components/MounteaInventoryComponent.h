// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "MounteaInventoryComponent.generated.h"

class UMounteaInventoryItem_Base;

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

	virtual bool HasItem_Implementation(const FItemRetrievalFilter& SearchFilter) override;
	virtual UMounteaInventoryItem_Base* FindItem_Implementation(const FItemRetrievalFilter& SearchFilter) override;
	virtual TArray<UMounteaInventoryItem_Base*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) override;
	
	virtual bool AddItem_Implementation(UMounteaInventoryItem_Base* NewItem) override;
	virtual bool AddItems_Implementation(TArray<UMounteaInventoryItem_Base*>& NewItems) override;
	virtual bool AddItemFromClass_Implementation(TSubclassOf<UMounteaInventoryItem_Base> ItemClass) override;
	virtual bool AddItemsFromClass_Implementation(TArray<TSubclassOf<UMounteaInventoryItem_Base>>& NewItemsClasses) override;

	virtual bool RemoveItem_Implementation(UMounteaInventoryItem_Base* AffectedItem) override;
	virtual bool RemoveItems_Implementation(TArray<UMounteaInventoryItem_Base*>& AffectedItems) override;

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

	virtual bool TryAddItem(UMounteaInventoryItem_Base* Item);
	virtual bool TryRemoveItem(UMounteaInventoryItem_Base* Item);

	UFUNCTION()
	void PostInventoryUpdated();
	UFUNCTION()
	void PostItemAdded(UMounteaInventoryItem_Base* Item, const FString& UpdateMessage);
	UFUNCTION()
	void PostItemRemoved(UMounteaInventoryItem_Base* Item, const FString& UpdateMessage);
	UFUNCTION()
	void PostItemUpdated(UMounteaInventoryItem_Base* Item, const FString& UpdateMessage);

#pragma endregion

#pragma region VARIABLES
	
protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemAdded;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemRemoved;
	UPROPERTY(BlueprintAssignable, Category="Mountea|Inventory")
	FOnItemUpdated OnItemUpdated;

protected:

	UPROPERTY(SaveGame, EditAnywhere, Category="1. Required", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties, MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryWBPInterface"))
	TSubclassOf<UUserWidget> InventoryWBPClass;

	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	UUserWidget* InventoryWBP = nullptr;

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false, ShowOnlyInnerProperties))
	TArray<UMounteaInventoryItem_Base*> Items;

	UPROPERTY()
	TArray<UMounteaInventoryItem_Base*> ClientLastReceivedItems;

private:

	UPROPERTY()
	int32 ReplicatedItemsKey;

#pragma endregion
	
};

#undef LOCTEXT_NAMESPACE