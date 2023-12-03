// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "WBP/Inventory/MounteaInventorySlotBaseWidget.h"

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
	FORCEINLINE FString GetSlotID() const { return SlotID; };

	UFUNCTION()
	void UpdateSlotID(const FGameplayTag& AffectedSlot);
	
protected:

	UPROPERTY(Category="1. Required", EditAnywhere, BlueprintReadOnly, meta=(GetOptions="GetSlotIDOptions"))
	FString SlotID;

private:

	UFUNCTION()
	static TArray<FString> GetSlotIDOptions();
};
