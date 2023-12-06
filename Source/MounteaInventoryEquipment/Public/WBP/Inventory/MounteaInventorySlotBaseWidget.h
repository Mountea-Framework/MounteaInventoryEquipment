// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaInventorySlotWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaInventorySlotBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventorySlotBaseWidget : public UMounteaBaseUserWidget, public IMounteaInventorySlotWBPInterface
{
	GENERATED_BODY()

public:

	virtual bool IsSlotEmpty_Implementation() const override;

protected:
	
	virtual FEventReply ResolveDrop_Implementation(UUserWidget* DroppedWidget, UObject* Payload) override;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Slot")
	FORCEINLINE FString GetSlotID() const;

protected:
	
	UPROPERTY(Category="Mountea|Required", EditAnywhere, BlueprintReadOnly, meta=(GetOptions="GetSlotIDOptions"))
	FString SlotID;

	UPROPERTY(Category="Mountea|Debug", EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FIntPoint SlotCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug", meta=(ExposeOnSpawn, DisplayThumbnail=false))
	TScriptInterface<IMounteaInventoryItemWBPInterface> OwningInventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug")
	FGuid ParentSlotGuid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Debug", meta=(DisplayThumbnail=false))
	UUserWidget* AttachedItemWidget = nullptr;
};
