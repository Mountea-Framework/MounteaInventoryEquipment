// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Helpers/InventoryHelpers.h"
#include "Interfaces/ActorInventoryInterface.h"

#include "ActorInventoryComponent.generated.h"

class UInventoryItem;

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
	virtual void AddItemToInventory(UInventoryItem* Item) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual void AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual TArray<UInventoryItem*> GetInventoryItems() const override {return InventoryItems; };

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void SetInventoryItems(const TArray<UInventoryItem*> Items) override {InventoryItems = Items; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual bool IsItemInInventory(const UInventoryItem* Item) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void LoadInventoryContent(const class UDataTable* SourceTable) override;
	
	virtual FOnInventoryUpdated& GetUpdateEventHandle() override;

#pragma region InventoryWidget

public:
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void SetInventoryWidgetClass(TSubclassOf<UInventoryWidget> NewInventoryWidgetClass) override;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual UInventoryWidget* GetInventoryWidgetPtr() const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	virtual TSubclassOf<UInventoryWidget> GetInventoryWidgetClass() const override;


#pragma endregion InventoryWidget

#pragma region Events

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

#pragma endregion Events

#pragma endregion InventoryInterface
	
protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|UI", meta=(BlueprintBaseOnly=""))
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|UI")
	UInventoryWidget* InventoryWidget = nullptr;
	
	/**
	 * List of Items that are currently in Inventory.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TArray<UInventoryItem*> InventoryItems;

private:

	UPROPERTY()
	class UActorInventoryManagerComponent* InventoryManager = nullptr;
};
