// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UActorInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class UInventoryItem;

/**
 * 
 */
class ACTORINVENTORYPLUGIN_API IActorInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	virtual void AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems) = 0;
	virtual void AddItemToInventory(UInventoryItem* Item) = 0;
	virtual void SetInventoryItems(const TArray<UInventoryItem*> Items) = 0;
	virtual TArray<UInventoryItem*> GetInventoryItems() const = 0;
	virtual bool IsItemInInventory(const UInventoryItem* Item) const = 0;
	virtual void LoadInventoryContent(const class UDataTable* SourceTable) = 0;
};
