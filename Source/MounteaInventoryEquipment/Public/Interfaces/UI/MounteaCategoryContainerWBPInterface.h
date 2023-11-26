// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaCategoryContainerWBPInterface.generated.h"

class UMounteaInventoryItemCategory;

UINTERFACE()
class UMounteaCategoryContainerWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaCategoryContainerWBPInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void CreateCategories(const TArray<UMounteaInventoryItemCategory*>& Categories);
};
