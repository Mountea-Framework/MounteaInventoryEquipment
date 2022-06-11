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

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FInventoryItemData GetItem() const
	{
		return Item;
	}
		
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItem(const FInventoryItemData& ItemValues)
	{
		Item = ItemValues;
	}

	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, ShowOnlyInnerProperties))
	FInventoryItemData Item;
};

#undef LOCTEXT_NAMESPACE