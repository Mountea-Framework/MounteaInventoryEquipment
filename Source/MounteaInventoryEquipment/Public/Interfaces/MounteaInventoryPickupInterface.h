// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryPickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMounteaInventoryPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Mountea Pickup Interface.
 *
 * Provides functionality to pickup/discard Items.
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryPickupInterface
{
	GENERATED_BODY()
	
public:

	/**
	 * Requests Item Pickup.
	 * Could fail, succeed or succeed with warning.
	 *
	 * Calls `TryPickupItem`.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	bool PickupItem(FString& ErrorMessage);
	bool PickupItem_Implementation(FString& ErrorMessage)
	{
		return TryPickupItem(ErrorMessage);
	};

	virtual bool TryPickupItem(FString& ErrorMessage) = 0;
};
