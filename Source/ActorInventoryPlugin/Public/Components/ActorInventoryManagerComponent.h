// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Definitions/InventoryCategory.h"
#include "Helpers/InventoryHelpers.h"

#include "ActorInventoryManagerComponent.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;

UCLASS(ClassGroup=(Inventory), Blueprintable, HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Manager", ShortTooltip="Inventory Manager responsible for Adding and Removing Items from Inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UActorInventoryManagerComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TArray<TSubclassOf<UInventoryCategory>> GetInventoryCategories() const
	{
		return AllowedCategories;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TArray<TSubclassOf<UInventoryItemRarity>> GetInventoryRarities() const
	{
		return AllowedRarities;
	}

	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	void ClearAllowedCategories();

	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	void SetAllowedCategories(const TArray<TSubclassOf<UInventoryCategory>> Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	void AddAllowedCategory(const TSubclassOf<UInventoryCategory> Category);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	void AddAllowedCategories(const TArray<TSubclassOf<UInventoryCategory>> Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	void RemoveAllowedCategory(const TSubclassOf<UInventoryCategory> Category);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	void RemoveAllowedCategories(const TArray<TSubclassOf<UInventoryCategory>> Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management")
	FORCEINLINE bool ContainsAllowedCategory(const TSubclassOf<UInventoryCategory> Category) const
	{
		return AllowedCategories.Contains(Category);
	}
	
protected:
	
	virtual void BeginPlay() override;

	/**
	 * List of allowed Categories to be displayed in the Inventory.
	 * Only selected Categories will be displayed.
	 * Only valid Categories will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TArray<TSubclassOf<UInventoryCategory>> AllowedCategories;

	/**
	 * List of allowed Rarities to be displayed in the Inventory.
	 * Only selected Rarities will be displayed.
	 * Only valid Rarities will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TArray<TSubclassOf<UInventoryItemRarity>> AllowedRarities;
};

