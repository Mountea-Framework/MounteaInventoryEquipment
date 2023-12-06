// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Input/Reply.h"
#include "MounteaEquipmentSlotWBPInterface.generated.h"

class IMounteaInventoryItemWBPInterface;
class IMounteaEquipmentWBPInterface;

struct FInventoryUpdateResult;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaEquipmentSlotWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaEquipmentSlotWBPInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	TScriptInterface<IMounteaEquipmentWBPInterface> GetOwningEquipment() const;
	virtual TScriptInterface<IMounteaEquipmentWBPInterface> GetOwningEquipment_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	void SetOwningEquipment(const TScriptInterface<IMounteaEquipmentWBPInterface>& NewOwningEquipment);
	virtual void SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentWBPInterface>& NewOwningEquipment) = 0;
	
};
