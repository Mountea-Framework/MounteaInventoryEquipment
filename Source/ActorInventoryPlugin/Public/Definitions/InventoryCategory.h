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
class ACTORINVENTORYPLUGIN_API UInventoryCategory : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetCategoryName() const
	{
		return CategoryName;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryCategory> GetParentCategory() const
	{
		return ParentCategory;
	}
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	FText CategoryName = LOCTEXT("InventoryCategory", "Default");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	UTexture2D* CategoryTexture = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TSubclassOf<UInventoryCategory> ParentCategory = nullptr;
};

#undef LOCTEXT_NAMESPACE