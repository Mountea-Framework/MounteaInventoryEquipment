// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaEquipmentWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaEquipmentBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentBaseWidget : public UMounteaBaseUserWidget, public IMounteaEquipmentWBPInterface
{
	GENERATED_BODY()

public:

	virtual TScriptInterface<IMounteaEquipmentInterface> GetOwningEquipment_Implementation() const override;
	virtual void SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentInterface>& NewOwningEquipment) override;
	virtual TArray<UUserWidget*> GetEquipmentSlotsWidgets_Implementation() const override;
	virtual void ResetEquipmentSlotsWidgets_Implementation() override;
	virtual void AddEquipmentSlotsWidget_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface")) UUserWidget* NewSlot) override;
	virtual void SetEquipmentSlotsWidgets_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface")) const TArray<UUserWidget*>& NewSlots) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="0. Debug", meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaEquipmentInterface> OwningEquipment;

	UPROPERTY(VisibleAnywhere, Category="0. Debug")
	TArray<UUserWidget*> EquipmentSlotWidgets;
};
