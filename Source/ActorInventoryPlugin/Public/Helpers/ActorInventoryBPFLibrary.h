// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	 * Returns Inventory Manager from PlayerController.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory", meta=(WorldContext="WorldContextObject", DefaultToSelf="WorldContextObject", CompactNodeTitle="Inventory Manager"))
	static class UActorInventoryManagerComponent* GetInventoryManager(const UObject* WorldContextObject);
	
};
