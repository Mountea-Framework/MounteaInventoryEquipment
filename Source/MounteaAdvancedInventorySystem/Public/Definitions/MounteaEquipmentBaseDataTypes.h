// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaEquipmentBaseDataTypes.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct FMounteaEquipmentSlotHeaderData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer TagContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
};