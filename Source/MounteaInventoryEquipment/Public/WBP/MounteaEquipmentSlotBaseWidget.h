// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventorySlotBaseWidget.h"
#include "Interfaces/UI/MounteaEquipmentSlotWBPInterface.h"
#include "MounteaEquipmentSlotBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentSlotBaseWidget : public UMounteaInventorySlotBaseWidget, public IMounteaEquipmentSlotWBPInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE FText GetSlotID() const { return SlotID; };
	
protected:

	UPROPERTY(Category="2. Required", EditInstanceOnly, BlueprintReadOnly, meta=(GetOptions="GetSlotIDOptions", NoResetToDefault))
	FText SlotID;

private:

	UFUNCTION()
	static TArray<FText> GetSlotIDOptions();
};
