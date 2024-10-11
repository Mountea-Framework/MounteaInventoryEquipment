// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/MounteaEquipmentSlotWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaBaseEquipmentSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaBaseEquipmentSlotWidget : public UMounteaBaseUserWidget, public IMounteaEquipmentSlotWBPInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE FString GetSlotID() const { return SlotID; };
	
protected:

	UPROPERTY(Category="2. Required", EditInstanceOnly, BlueprintReadOnly, meta=(GetOptions="GetSlotIDOptions", NoResetToDefault))
	FString SlotID;

private:

	UFUNCTION()
	static TArray<FString> GetSlotIDOptions();
};
