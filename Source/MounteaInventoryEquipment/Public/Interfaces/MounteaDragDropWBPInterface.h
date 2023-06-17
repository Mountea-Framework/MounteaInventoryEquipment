// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaDragDropWBPInterface.generated.h"

class IMounteaInventoryItemWBPInterface;
class IMounteaInventoryWBPInterface;
class UMounteaInventoryItemBase;

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
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
	void ProcessWidgetCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void RequestCreateDragDrop(const TScriptInterface<IMounteaInventoryItemWBPInterface>& OwningItem, const TScriptInterface<IMounteaInventoryWBPInterface>& OwningInventory);
};
