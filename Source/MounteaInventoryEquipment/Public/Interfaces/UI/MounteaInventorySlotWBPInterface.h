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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventory() const;
	virtual TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventory_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Equipment")
	void SetOwningInventory(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory);
	virtual void SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment")
	FEventReply ResolveDrop(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* DroppedWidget, UObject* Payload);
	virtual FEventReply ResolveDrop_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* DroppedWidget, UObject* Payload) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment")
	bool IsSlotEmpty() const;
	virtual bool IsSlotEmpty_Implementation() const = 0;

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
	
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment")
	void OnInventorySlotUpdated(const FString& UpdateMessage, const UUserWidget* AttachedChildWidget);



	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void OccupySlot(const TScriptInterface<IMounteaInventoryItemWBPInterface>& Item);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void EmptySlot();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	FIntPoint GetSlotRoots() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void SetSlotRoots(const FIntPoint& NewRoots);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryItemWBPInterface> GetOccupyingItem() const;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void OnShadowCasted(const bool bIsValid);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void OnShadowLifted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void OnDragSolutionFound(const UDragDropOperation* Operation, const bool bResult);
};
