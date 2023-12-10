// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaBaseWidgetInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaInventoryItemBaseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemWidgetChanged, UMounteaInventoryItemBaseWidget*, NewSlot);

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBaseWidget : public UMounteaBaseUserWidget, public IMounteaInventoryItemWBPInterface, public IMounteaBaseWidgetInterface
{
	GENERATED_BODY()

public:

	virtual TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventory_Implementation() const override;
	virtual void SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory) override;

	virtual FItemSlot GetItem_Implementation() const override;
	virtual UMounteaBaseUserWidget* GetItemUI_Implementation() override;
	virtual FIntPoint GetRootCoords_Implementation() const override;
	virtual FIntPoint GetItemDimensions_Implementation() const override;
	virtual bool SaveRootCoords_Implementation(const FIntPoint& NewCoords) override;
	virtual bool MoveToNewCoords_Implementation(const FIntPoint& NewCoords) override;
	virtual bool ReleaseOldCoords_Implementation(const FIntPoint& OldCoords) override;
	virtual bool CanBeDragged_Implementation() const override;
	virtual void OnDropped_Implementation() override;
	virtual void OnDragged_Implementation() override;

protected:

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaInventoryWBPInterface> OwningInventory;

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FItemSlot ItemSlot;

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FIntPoint SlotCoordinates;

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FIntPoint SlotDimensions;
};
