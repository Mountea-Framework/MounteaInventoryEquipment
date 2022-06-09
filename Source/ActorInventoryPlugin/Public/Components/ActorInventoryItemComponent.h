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
	FORCEINLINE TSubclassOf<UInventoryItem> GetItemDefinition() const
	{
		return ItemDefinition;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetItemDefinition(const TSubclassOf<UInventoryItem> Item);

protected:

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UInventoryItem> ItemDefinition;
};
