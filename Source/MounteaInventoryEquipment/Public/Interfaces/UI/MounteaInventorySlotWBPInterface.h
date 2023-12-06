// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "UObject/Interface.h"
#include "MounteaInventorySlotWBPInterface.generated.h"

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment")
	FEventReply ResolveDrop(UUserWidget* DroppedWidget, UObject* Payload);
	virtual FEventReply ResolveDrop_Implementation(UUserWidget* DroppedWidget, UObject* Payload) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory & Equipment")
	bool IsSlotEmpty() const;
	virtual bool IsSlotEmpty_Implementation() const = 0;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Mountea|Inventory & Equipment")
	void OnEquipmentSlotUpdated(const FString& UpdateMessage, const UUserWidget* AttachedChildWidget);





	
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
