// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/InventoryHelpers.h"

#include "InventoryItem.generated.h"


#define LOCTEXT_NAMESPACE "Inventory Item"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItem : public UObject
{
	GENERATED_BODY()

	UInventoryItem(){};

	explicit UInventoryItem(FInventoryItemData& ItemData)
	: Item(ItemData)
	{};

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FInventoryItemData& GetItem()
	{
		return Item;
	}
		
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItem(const FInventoryItemData& ItemValues)
	{
		Item = ItemValues;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool IsValidItem() const
	{
		return
		Item.ItemCategory != nullptr &&
		Item.ItemRarity != nullptr &&
		Item.ItemQuantityData.Quantity <= Item.ItemQuantityData.MaxQuantity;
	}
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, ShowOnlyInnerProperties))
	FInventoryItemData Item;

protected:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};

#undef LOCTEXT_NAMESPACE