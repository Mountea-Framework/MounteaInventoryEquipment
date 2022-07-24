// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryCategoryTooltip.generated.h"

class UInventoryCategoryWidget;

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryCategoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryCategoryWidget* GetOwningCategoryWidget() const
	{
		return OwningCategoryWidget;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void InitializeTooltip(UInventoryCategoryWidget* NewOwningCategoryWidget);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	UInventoryCategoryWidget* OwningCategoryWidget = nullptr;
	
};
