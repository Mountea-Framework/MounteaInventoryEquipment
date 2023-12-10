// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "UObject/Interface.h"
#include "MounteaInventorySlotWBPInterface.generated.h"

class IMounteaInventoryWBPInterface;
class IMounteaInventoryItemWBPInterface;
class UDragDropOperation;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventorySlotWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventorySlotWBPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	FEventReply ResolveDrop(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload);
	virtual FEventReply ResolveDrop_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	bool IsSlotEmpty() const;
	virtual bool IsSlotEmpty_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	bool CanAttach(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const;
	virtual bool CanAttach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	bool CanDetach(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const;
	virtual bool CanDetach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	FInventoryUpdateResult AttachItemToSlot(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToAttach);
	virtual FInventoryUpdateResult AttachItemToSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToAttach) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	FInventoryUpdateResult DetachItemToSlot(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToDetach);
	virtual FInventoryUpdateResult DetachItemFromSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToDetach) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Slot")
	bool CanDrop(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload) const;
	virtual bool CanDrop_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload) const = 0;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void OnInventorySlotUpdated(const FString& UpdateMessage, const UUserWidget* AttachedChildWidget);



	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void OccupySlot(const TScriptInterface<IMounteaInventoryItemWBPInterface>& Item);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void EmptySlot();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	FIntPoint GetSlotRoots() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void SetSlotRoots(const FIntPoint& NewRoots);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	TScriptInterface<IMounteaInventoryItemWBPInterface> GetOccupyingItem() const;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void OnShadowCasted(const bool bIsValid);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void OnShadowLifted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment|Slot")
	void OnDragSolutionFound(const UDragDropOperation* Operation, const bool bResult);
};
