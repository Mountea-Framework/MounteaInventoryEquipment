// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryItemSlotDrag.generated.h"

class UInventoryItemSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropped, const FInventorySlotData&, DataAfterDropped);

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryItemSlotDrag : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Inventory")
	FOnDropped OnDropped;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryItemSlot* GetSourceSlotData() const
	{
		return SourceSlotData;
	}

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	UInventoryItemSlot* SourceSlotData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	FIntPoint SourceSlotCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	FIntPoint TargetSlotCoordinates;
};
