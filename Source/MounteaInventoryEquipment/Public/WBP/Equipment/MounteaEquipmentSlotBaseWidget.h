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

	virtual void SetOwningEquipment_Implementation(const TScriptInterface<IMounteaEquipmentWBPInterface>& NewOwningEquipment) override;
	virtual TScriptInterface<IMounteaEquipmentWBPInterface> GetOwningEquipment_Implementation() const override;
	
	virtual bool IsSlotEmpty_Implementation() const override;

	virtual FInventoryUpdateResult AttachItemToSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToAttach) override;
	virtual FInventoryUpdateResult DetachItemFromSlot_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* ItemToDetach) override;

	virtual bool CanAttach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* NewChildWidget, FInventoryUpdateResult& OutResult) const override;
	virtual bool CanDetach_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.MounteaInventoryItemWBPInterface")) UUserWidget* OldChildWidget, FInventoryUpdateResult& OutResult) const override;

	virtual bool CanDrop_Implementation(UPARAM(meta=(MustImplement="/Script/MounteaInventoryEquipment.IMounteaDragDropWBPInterface")) UUserWidget* PayloadWidget, UObject* Payload) const;
	
protected:

	virtual FEventReply ResolveDrop_Implementation(UUserWidget* PayloadWidget, UObject* Payload) override;	

public:
	
	UFUNCTION()
	void UpdateSlotID(const FGameplayTag& AffectedSlot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Slot")
	FORCEINLINE FString GetSlotID() const
	{ return SlotID; };

private:

	UFUNCTION()
	static TArray<FString> GetSlotIDOptions();
	
protected:

	UPROPERTY(Category="Mountea|Required", EditAnywhere, BlueprintReadOnly, meta=(GetOptions="GetSlotIDOptions"))
	FString SlotID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug", meta=(ExposeOnSpawn, DisplayThumbnail=false))
	TScriptInterface<IMounteaEquipmentWBPInterface> OwningEquipmentWidget;
};
