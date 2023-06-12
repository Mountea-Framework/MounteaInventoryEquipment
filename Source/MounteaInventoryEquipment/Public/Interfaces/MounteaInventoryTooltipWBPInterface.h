// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryTooltipWBPInterface.generated.h"

class UMounteaItemAdditionalData;

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
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
	void UpdateTooltip(const UMounteaItemAdditionalData* Data);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void ShowTooltip();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void RemoveTooltip();
};
