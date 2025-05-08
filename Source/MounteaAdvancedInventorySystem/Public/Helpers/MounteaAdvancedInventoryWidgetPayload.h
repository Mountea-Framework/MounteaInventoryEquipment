// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MounteaAdvancedInventoryWidgetPayload.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryWidgetPayload : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	TArray<FGuid> PayloadData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	int32 PayloadIndex = INDEX_NONE;
};
