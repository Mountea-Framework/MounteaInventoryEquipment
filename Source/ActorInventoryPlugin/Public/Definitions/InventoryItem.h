// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;

#define LOCTEXT_NAMESPACE "Inventory Item"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItem : public UDataAsset
{
	GENERATED_BODY()

#pragma region GETTERS
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryCategory> GetItemCategory() const
	{
		return ItemCategory;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryItemRarity> GetItemCRarity() const
	{
		return ItemRarity;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UTexture2D* GetItemThumbnail() const
	{
		return ItemThumbnail;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetItemTittle() const
	{
		return ItemTittle;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetItemDescription() const
	{
		return ItemDescription;
	}

#pragma endregion GETTERS

#pragma region SETTERS

public:
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemCategory(const TSubclassOf<UInventoryCategory> Category);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemRarity(const TSubclassOf<UInventoryItemRarity> Rarity);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemThumbnail(UTexture2D* Thumbnail);

#pragma endregion SETTERS

#pragma region VARIABLES
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TSubclassOf<UInventoryCategory> ItemCategory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TSubclassOf<UInventoryItemRarity> ItemRarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	UTexture2D* ItemThumbnail = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	FText ItemTittle = LOCTEXT("ItemTittle", "Default Object");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(MultiLine=true))
	FText ItemDescription = LOCTEXT("ItemDescription", "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

#pragma endregion VARIABLES
};

#undef LOCTEXT_NAMESPACE