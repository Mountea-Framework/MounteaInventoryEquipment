// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;

/**
 * 
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryCategory> GetItemCategory() const
	{
		return ItemCategory;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemCategory(const TSubclassOf<UInventoryCategory> Category);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryItemRarity> GetItemCRarity() const
	{
		return ItemRarity;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemRarity(const TSubclassOf<UInventoryItemRarity> Rarity);

protected:

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UInventoryCategory> ItemCategory;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UInventoryItemRarity> ItemRarity;
};
