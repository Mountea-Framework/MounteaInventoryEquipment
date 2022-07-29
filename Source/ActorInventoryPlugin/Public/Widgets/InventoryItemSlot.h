// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryItemSlot.generated.h"

class UInventoryWidget;

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Item Slot")
	FORCEINLINE FInventorySlotData GetItemData() const {return ItemSlotData; };

	// Event called once this Slot has been updated.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory|Item Slot")
	void OnSlotUpdated(const FInventorySlotData& SlotData);

	// Event called once this Slot has been updated.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory|Item Slot")
	void OnSlotInspected();
	
protected:

	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory|Item Slot")
	FORCEINLINE bool DoesHaveItem() const
	{
		return ItemSlotData.Item != nullptr;
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Item Slot")
	void ClearItem()
	{
		ItemSlotData = FInventorySlotData();
	}

private:
	
	void UpdateItemSlot(const FInventorySlotData& SlotData);
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	FInventorySlotData ItemSlotData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ExposeOnSpawn = true))
	UInventoryWidget* OwningInventoryWidget = nullptr;
};
