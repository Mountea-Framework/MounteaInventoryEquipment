// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaInventoryTooltipWBPInterface.generated.h"

class IMounteaInventoryItemWBPInterface;

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
	void UpdateTooltip(const TScriptInterface<IMounteaInventoryItemWBPInterface>& Item);
};
