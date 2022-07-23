// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryCategoryWidget.generated.h"

class UInventoryWidget;
class UInventoryCategory;

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryCategoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryCategory* GetCategoryDefinition() const
	{
		return ParentCategory;
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void OnCategorySelected();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void OnSelectedCategoryUpdated(const FGuid& SelectedCategoryGUID);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	UInventoryCategory* ParentCategory = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true))
	UInventoryWidget* OwningInventory = nullptr;
	
};
