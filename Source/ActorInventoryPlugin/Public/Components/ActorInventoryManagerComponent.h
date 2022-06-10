// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Definitions/InventoryCategory.h"
#include "Helpers/InventoryHelpers.h"
#include "Helpers/ActorInventoryPluginLog.h"

#include "ActorInventoryManagerComponent.generated.h"

class UInventoryCategory;
class UInventoryItemRarity;

UCLASS(ClassGroup=(Inventory), Blueprintable, HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Manager", ShortTooltip="Inventory Manager responsible for Adding and Removing Items from Inventory."))
class ACTORINVENTORYPLUGIN_API UActorInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UActorInventoryManagerComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management")
	FORCEINLINE TSet<TSubclassOf<UInventoryCategory>> GetInventoryCategories() const
	{
		return AllowedCategories;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Management")
	FORCEINLINE TSet<TSubclassOf<UInventoryItemRarity>> GetInventoryRarities() const
	{
		return AllowedRarities;
	}

	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void ClearAllowedCategories();

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void SetAllowedCategories(const TSet<TSubclassOf<UInventoryCategory>>& Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void AddAllowedCategory(const TSubclassOf<UInventoryCategory>& Category);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void AddAllowedCategories(const TSet<TSubclassOf<UInventoryCategory>>& Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void RemoveAllowedCategory(const TSubclassOf<UInventoryCategory>& Category);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	void RemoveAllowedCategories(const TSet<TSubclassOf<UInventoryCategory>>& Categories);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE bool ContainsAllowedCategory(const TSubclassOf<UInventoryCategory>& Category) const
	{
		return AllowedCategories.Contains(Category);
	}

	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void ClearAllowedRarities();

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void SetAllowedRarities(const TSet<TSubclassOf<UInventoryItemRarity>>& Rarities);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void AddAllowedRarity(const TSubclassOf<UInventoryItemRarity>& Rarity);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void AddAllowedRarities(const TSet<TSubclassOf<UInventoryItemRarity>>& Rarities);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void RemoveAllowedRarity(const TSubclassOf<UInventoryItemRarity>& Rarity);

	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Rarity")
	void RemoveAllowedRarities(const TSet<TSubclassOf<UInventoryItemRarity>>& Rarities);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Management|Categories")
	FORCEINLINE bool ContainsAllowedRarity(const TSubclassOf<UInventoryItemRarity>& Rarity) const
	{
		return AllowedRarities.Contains(Rarity);
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory", meta=(DeprecatedFunction, DeprecatedMessage="Use this function with caution as this might be really performance heavy."))
	void UpdateCategories();
	
protected:
	
	virtual void BeginPlay() override;

	//TODO: TSet

	/**
	 * If Category is added to Allowed Categories, should we allow its parent Categories as well?
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	uint8 bAutoAllowParentCategories : 1;
	
	/**
	 * List of allowed Categories to be displayed in the Inventory.
	 * Only selected Categories will be displayed.
	 * Only valid Categories will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory", NoClear, meta=(NoResetToDefault, NoElementDuplicate, BlueprintBaseOnly))
	TSet<TSubclassOf<UInventoryCategory>> AllowedCategories;

	/**
	 * List of allowed Rarities to be displayed in the Inventory.
	 * Only selected Rarities will be displayed.
	 * Only valid Rarities will be displayed.
	 * Display order is equal order in Array.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory", NoClear, meta=(NoResetToDefault, NoElementDuplicate, BlueprintBaseOnly))
	TSet<TSubclassOf<UInventoryItemRarity>> AllowedRarities;

private:
	
	void AddParentCategory(const TSubclassOf<UInventoryCategory>& Category);
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
};
