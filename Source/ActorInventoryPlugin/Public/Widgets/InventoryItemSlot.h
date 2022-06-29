// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryItemSlot.generated.h"


/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGIN_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool DoesHaveItem() const
	{
		return ItemSlotData.Item != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void ClearItem()
	{
		ItemSlotData.Item = nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void SlotUpdated(const FInventorySlotData& Data);

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	FInventorySlotData ItemSlotData;
};
