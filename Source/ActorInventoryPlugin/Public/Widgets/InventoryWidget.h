// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/ActorInventoryInterface.h"
#include "InventoryWidget.generated.h"

class UActorInventoryInterface;

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual bool Initialize() override;

public:

	/**
	 * @brief 
	 * @param InventoryInterface 
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Inventory")
	void RefreshWidget(UActorComponent* InventoryInterface);

	/**
	 * @brief 
	 * @param InventorySlot 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory")
	void SaveInventorySlot(const FInventorySlotData& InventorySlot);

protected:

	/**
	 * @brief 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TScriptInterface<IActorInventoryInterface> OwningInventory = nullptr;
};
