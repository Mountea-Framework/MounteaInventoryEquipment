// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryTooltipWBPInterface.generated.h"

class UMounteaInventoryItemBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaTooltipWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaTooltipWBPInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void UpdateTooltip(const UMounteaInventoryItemBase* Item);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void ProcessCommand(const FString& Command);
};
