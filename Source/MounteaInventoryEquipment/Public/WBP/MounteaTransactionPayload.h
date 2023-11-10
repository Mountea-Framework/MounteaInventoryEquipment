// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MounteaTransactionPayload.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Transaction Payload (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaTransactionPayload : public UObject
{
	GENERATED_BODY()
};
