// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGIN_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool DoesHaveItem() const
	{
		return Item != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void ClearItem()
	{
		Item = nullptr;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	class UInventoryItem* Item = nullptr;
	
};
