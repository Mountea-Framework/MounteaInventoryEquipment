﻿// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaInventorySlotWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaInventorySlotBaseWidget.generated.h"

class IMounteaInventoryWBPInterface;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventorySlotBaseWidget : public UMounteaBaseUserWidget, public IMounteaInventorySlotWBPInterface
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

public:

	virtual void SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningEquipment) override;
	virtual TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventory_Implementation() const override;
	
	virtual bool IsSlotEmpty_Implementation() const override;

	virtual FInventoryUpdateResult AttachItemToSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToAttach) override;
	virtual FInventoryUpdateResult DetachItemFromSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToDetach) override;

	virtual bool CanAttach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const override;
	virtual bool CanDetach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const override;

	virtual bool CanDrop_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload) const override;
	
protected:
	
	virtual FEventReply ResolveDrop_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload) override;

protected:
	
	UPROPERTY(Category="Mountea|Debug", EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FIntPoint SlotCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug", meta=(ExposeOnSpawn, DisplayThumbnail=false))
	TScriptInterface<IMounteaInventoryWBPInterface> OwningInventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug")
	FGuid ParentSlotGuid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug", meta=(DisplayThumbnail=false))
	UUserWidget* AttachedItemWidget = nullptr;
};
