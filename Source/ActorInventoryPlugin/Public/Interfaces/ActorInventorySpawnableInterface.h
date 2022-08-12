// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorInventorySpawnableInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnActorRequested);

// This class does not need to be modified.
UINTERFACE()
class UActorInventorySpawnableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Actor Inventory Spawnable Interface.
 *
 * This Interface takes care of spawning Actors from Items.
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/ActorInventorySpawnableInterface
 */
class ACTORINVENTORYPLUGIN_API IActorInventorySpawnableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FOnSpawnActorRequested& GetSpawnActorRequestedHandle() const = 0;
};
