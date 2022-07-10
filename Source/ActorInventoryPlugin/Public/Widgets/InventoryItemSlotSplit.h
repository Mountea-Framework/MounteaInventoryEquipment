// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlotSplit.generated.h"

class UInventoryItemSlot;
class UInventoryWidget;

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryItemSlotSplit : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void Init(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool IsValid() const
	{
		return OriginSlot != nullptr && TargetSlot != nullptr;
	}
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryWidget* OwningWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryItemSlot* OriginSlot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryItemSlot* TargetSlot = nullptr;
};
