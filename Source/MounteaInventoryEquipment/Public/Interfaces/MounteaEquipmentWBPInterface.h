// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaEquipmentWBPInterface.generated.h"

class UMounteaEquipmentSlot;
class IMounteaEquipmentInterface;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaEquipmentWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaEquipmentWBPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	TScriptInterface<IMounteaEquipmentInterface> GetOwningEquipment() const;
	virtual TScriptInterface<IMounteaEquipmentInterface> GetOwningEquipment_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	void SetOwningEquipment(const TScriptInterface<IMounteaEquipmentInterface>& OwningEquipment);
	virtual void SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentInterface>& OwningEquipment) = 0;
};
