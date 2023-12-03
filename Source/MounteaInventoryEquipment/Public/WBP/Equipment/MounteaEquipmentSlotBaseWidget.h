// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "WBP/Inventory/MounteaInventorySlotBaseWidget.h"

#include "Interfaces/UI/MounteaEquipmentSlotWBPInterface.h"

#include "MounteaEquipmentSlotBaseWidget.generated.h"

class IMounteaEquipmentWBPInterface;
/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentSlotBaseWidget : public UMounteaInventorySlotBaseWidget, public IMounteaEquipmentSlotWBPInterface
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	virtual TScriptInterface<IMounteaEquipmentWBPInterface> GetOwningEquipment_Implementation() const override;
	virtual void SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentWBPInterface>& NewOwningEquipment) override;
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|EquipmentSlot")
	FORCEINLINE FString GetSlotID() const { return SlotID; };

	UFUNCTION()
	void UpdateSlotID(const FGameplayTag& AffectedSlot);
	
protected:

	UPROPERTY(Category="1. Required", EditAnywhere, BlueprintReadOnly, meta=(GetOptions="GetSlotIDOptions"))
	FString SlotID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="0. Debug", meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaEquipmentWBPInterface> OwningEquipmentWidget;

private:

	UFUNCTION()
	static TArray<FString> GetSlotIDOptions();
};
