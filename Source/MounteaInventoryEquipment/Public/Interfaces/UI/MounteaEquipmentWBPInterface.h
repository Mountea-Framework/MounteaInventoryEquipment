// Copyright Dominik Morse 2023. All Rights Reserved.

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
	void SetOwningEquipment(const TScriptInterface<IMounteaEquipmentInterface>& NewOwningEquipment);
	virtual void SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentInterface>& NewOwningEquipment) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	TArray<UUserWidget*> GetEquipmentSlotsWidgets() const;
	virtual TArray<UUserWidget*> GetEquipmentSlotsWidgets_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	void ResetEquipmentSlotsWidgets();
	virtual void ResetEquipmentSlotsWidgets_Implementation() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	void AddEquipmentSlotsWidget(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface")) UUserWidget* NewSlot);
	virtual void AddEquipmentSlotsWidget_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface")) UUserWidget* NewSlot) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	void SetEquipmentSlotsWidgets(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface")) const TArray<UUserWidget*>& NewSlots);
	virtual void SetEquipmentSlotsWidgets_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaEquipmentSlotWBPInterface")) const TArray<UUserWidget*>& NewSlots) = 0;
};
