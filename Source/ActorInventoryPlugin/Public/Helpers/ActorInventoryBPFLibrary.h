// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryHelpers.h"
#include "Components/ActorInventoryManagerComponent.h"
#include "Definitions/InventoryCategory.h"
#include "Definitions/InventoryItem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorInventoryBPFLibrary.generated.h"

/**
 * Static class with useful Inventory utility functions that can be called from both Blueprint and C++
 */
UCLASS()
class ACTORINVENTORYPLUGIN_API UActorInventoryBPFLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns Inventory Manager from GameState or nullptr if not specified.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject", CompactNodeTitle="Inventory Manager"))
	static class UActorInventoryManagerComponent* GetInventoryManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static bool HasValidItem(const FInventorySlotData& ItemData)
	{
		return ItemData.Item != nullptr && ItemData.Item->IsValidItem();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static bool IsGenericCategory(const UInventoryCategory* Category)
	{
		return Category != nullptr && Category->GetCategoryData().IsAllCategories();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static UInventoryCategory*  GetGenericCategory(const UObject* WorldContextObject)
	{
		const UActorInventoryManagerComponent* InventoryManager = GetInventoryManager(WorldContextObject);
		if (InventoryManager)
		{
			for (const auto Itr : InventoryManager->GetAllowedCategories())
			{
				if (Itr && Itr->GetCategoryData().IsAllCategories())
				{
					return Itr;
				}
			}
		}
		
		return nullptr;
	}
};
