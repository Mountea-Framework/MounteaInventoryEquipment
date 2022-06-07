// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActorInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTORINVENTORYPLUGIN_API IActorInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void AddItemsToInventory(const TArray<UObject*> ListOfItems) = 0;
	virtual void AddItemToInventory(UObject* const Item) = 0;
	virtual TArray<UObject*> GetInventoryItems() const = 0;
	virtual UObject* GetInventoryItem() const = 0;
};
