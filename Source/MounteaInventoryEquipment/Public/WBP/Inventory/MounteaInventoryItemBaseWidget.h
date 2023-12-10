// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaBaseWidgetInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaInventoryItemBaseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemWidgetMoved, UMounteaInventoryItemBaseWidget*, MovedItem, const FIntPoint&, OldCoords);

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemBaseWidget : public UMounteaBaseUserWidget, public IMounteaInventoryItemWBPInterface, public IMounteaBaseWidgetInterface
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	virtual TScriptInterface<IMounteaInventoryWBPInterface> GetOwningInventoryWidget_Implementation() const override;
	virtual void SetOwningInventoryWidget_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory) override;

	virtual bool IsDirty_Implementation() const override;
	virtual void SetDirty_Implementation(const bool NewDirtyState) override;
	virtual FItemSlot GetItem_Implementation() const override;
	virtual UMounteaBaseUserWidget* GetItemUI_Implementation() override;
	virtual FIntPoint GetRootCoords_Implementation() const override;
	virtual FIntPoint GetItemDimensions_Implementation() const override;
	virtual bool SaveRootCoords_Implementation(const FIntPoint& NewCoords) override;
	virtual bool MoveToNewCoords_Implementation(const FIntPoint& NewCoords) override;
	virtual bool ReleaseOldCoords_Implementation(const FIntPoint& OldCoords) override;
	virtual TArray<FIntPoint> GetOccupyingCoords_Implementation() const override;
	virtual bool CanBeDragged_Implementation() const override;
	virtual void OnDropped_Implementation() override;
	virtual void OnDragged_Implementation() override;

	FOnItemWidgetMoved& GetOnItemMovedHandle()
	{ return OnItemWidgetMoved; };
	FOnItemWidgetMoved& GetPostItemMovedHandle()
	{ return PostItemWidgetMoved; };

protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea|Item")
	FOnItemWidgetMoved OnItemWidgetMoved;

	UPROPERTY(BlueprintAssignable, Category="Mountea|Item")
	FOnItemWidgetMoved PostItemWidgetMoved;

protected:

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaInventoryWBPInterface> OwningInventory;

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FItemSlotStack ItemStack;

	/**
	 * Current coordinates of this Slot. Useful for Grid Inventories.
	 * If Item occupies more than 1 slot, this is only the root.
	 */
	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FIntPoint SlotCoordinates;

	/**
	 * UI Size of a specific item. Useful for Grid Inventories.
	 * If the Item takes more than 1 slot, to get all Slots it occupies use `GetOccupyingCoords`.
	 */
	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FIntPoint SlotDimensions;

private:

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bIsDirty : 1;
};
