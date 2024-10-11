// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaDragDropWBPInterface.generated.h"

class IMounteaInventoryItemWBPInterface;
class IMounteaInventoryWBPInterface;
class UMounteaInventoryItem;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaDragDropWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaDragDropWBPInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryItemWBPInterface> GetItemWBP() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void RequestCreateDragDrop(const TScriptInterface<IMounteaInventoryItemWBPInterface>& OwningItem, const TScriptInterface<IMounteaInventoryWBPInterface>& OwningInventory);
};
