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
UCLASS()
class ACTORINVENTORYPLUGIN_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual bool Initialize() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory")
	void RefreshWidget(UActorComponent* InventoryInterface);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TScriptInterface<IActorInventoryInterface> OwningInventory = nullptr;
};
