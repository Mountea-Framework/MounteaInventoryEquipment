// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
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

	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	bool CanAttach(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const;
	virtual bool CanAttach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	bool CanDetach(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const;
	virtual bool CanDetach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	FInventoryUpdateResult AttachItemToSlot(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToAttach);
	virtual FInventoryUpdateResult AttachItemToSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToAttach) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	FInventoryUpdateResult DetachItemToSlot(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToDetach);
	virtual FInventoryUpdateResult DetachItemFromSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToDetach) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	FEventReply ResolveDrop(UUserWidget* DroppedWidget, UObject* Payload);
	virtual FEventReply ResolveDrop_Implementation(UUserWidget* DroppedWidget, UObject* Payload) = 0;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Equipment")
	void OnEquipmentSlotUpdated(const FString& UpdateMessage, const UUserWidget* AttachedChildWidget);
};
