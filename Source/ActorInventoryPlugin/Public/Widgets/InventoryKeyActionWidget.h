// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InventoryKeyActionWidget.generated.h"

class UInventoryKeyAction;
class UInventoryWidget;
class UInventoryItem;

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGIN_API UInventoryKeyActionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryKeyAction* GetParentKeyAction() const { return ParentKeyAction; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryWidget* GetParentInventoryWidget() const { return ParentInventoryWidget; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryItem* GetParentInventoryItem() const { return ParentInventoryItem; };

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryKeyAction* ParentKeyAction = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryWidget* ParentInventoryWidget = nullptr;

	// What Item does this Action origin from
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryItem* ParentInventoryItem = nullptr;
};
