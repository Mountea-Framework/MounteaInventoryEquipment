// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Helpers/InventoryHelpers.h"

#include "InventoryItem.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;

#define LOCTEXT_NAMESPACE "Inventory Item"

// TODO: Delete and use Inventory Instead

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItem : public UObject
{
	GENERATED_BODY()

	UInventoryItem(){};

	explicit UInventoryItem(const FInventoryItemData& ItemData) : Item(ItemData){};

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FInventoryItemData GetItem() const
	{
		return Item;
	}
		
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItem(const FInventoryItemData& ItemValues);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, ShowOnlyInnerProperties))
	FInventoryItemData Item;
};

#undef LOCTEXT_NAMESPACE