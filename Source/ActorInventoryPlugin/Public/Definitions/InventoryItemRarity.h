// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/InventoryHelpers.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemRarity.generated.h"

#define LOCTEXT_NAMESPACE "InventoryRarity"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItemRarity : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetRarityName() const
	{
		return InventoryRarityData.RarityName;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FLinearColor GetRarityColor() const
	{
		return InventoryRarityData.RarityColor;
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetRarityName(const FText& Name);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetRarityColor(const FLinearColor& Color);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, NoResetToDefault=true, ShowOnlyInnerProperties=true))
	FInventoryRarityData InventoryRarityData;
};

#undef LOCTEXT_NAMESPACE