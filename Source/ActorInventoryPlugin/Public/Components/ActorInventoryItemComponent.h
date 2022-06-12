// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Helpers/InventoryHelpers.h"

#include "ActorInventoryItemComponent.generated.h"

class UInventoryItem;

/**
 * Helper Component which defines that Actor is Inventory Item.
 * Usable for Pickups, like Weapons.
 */
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), HideCategories=(Collision, AssetUserData, Cooking, ComponentTick, Activation) )
class ACTORINVENTORYPLUGIN_API UActorInventoryItemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FInventoryItemData GetItemDefinition() const
	{
		return Item;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemDefinition(const FInventoryItemData NewItem);

protected:

	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(ShowOnlyInnerProperties="true"))
	FInventoryItemData Item;
};
