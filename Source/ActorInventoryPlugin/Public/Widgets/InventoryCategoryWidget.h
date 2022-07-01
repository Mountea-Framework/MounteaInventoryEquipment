// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryCategoryWidget.generated.h"

class UInventoryCategory;

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGIN_API UInventoryCategoryWidget : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryCategory* GetCategoryDefinition() const
	{
		return ParentCategory;
	}

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	UInventoryCategory* ParentCategory = nullptr;
	
};
