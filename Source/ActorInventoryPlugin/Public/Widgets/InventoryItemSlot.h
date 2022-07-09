// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryItemSlot.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	// Event called once this Slot has been updated.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void OnSlotUpdated(const FInventorySlotData& SlotData);

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
		ItemSlotData = FInventorySlotData();
	}

private:
	
	void UpdateItemSlot(const FInventorySlotData& Data);
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	FInventorySlotData ItemSlotData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ExposeOnSpawn = true))
	class UInventoryWidget* OwningInventoryWidget = nullptr;
};
