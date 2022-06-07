// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryCategory.generated.h"

#define LOCTEXT_NAMESPACE "Inventory Category"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryCategory : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetCategoryName() const
	{
		return CategoryName;
	}

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	FText CategoryName = LOCTEXT("InventoryCategory", "Default");

};

#undef LOCTEXT_NAMESPACE