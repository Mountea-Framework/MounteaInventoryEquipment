// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "InventoryHelpers.h"
#include "Definitions/InventoryCategory.h"
#include "Components/ActorInventoryManagerComponent.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorInventoryBPFLibrary.generated.h"

class UInventoryCategory;
class UInventoryKeyAction;
class UInventoryTransaction;

/**
 * Static class with useful Inventory utility functions that can be called from both Blueprint and C++
 */
UCLASS(Blueprintable, Category="Inventory")
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
	static bool HasValidItem(const FInventorySlotData& ItemData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static bool IsValidCategory(const UObject* WorldContextObject, UInventoryCategory* Category);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static UInventoryCategory*  GetGenericCategory(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static TSubclassOf<UInventoryKeyAction> FindKeyAction(const UObject* WorldContextObject, const FGuid& Guid, const UInventoryCategory* Category);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static bool  IsValidKeyAction(const UObject* WorldContextObject, const FGuid& Guid);

};
