// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryPickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
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
		return true;
	};

	//TODO give it more context
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	bool DropItem(FString& ErrorMessage);
	bool DropItem_Implementation(FString& ErrorMessage)
	{
		return true;
	};
};
