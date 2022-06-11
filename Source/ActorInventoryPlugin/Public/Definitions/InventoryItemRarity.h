// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemRarity.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItemRarity : public UDataAsset
{
	GENERATED_BODY()

	UInventoryItemRarity();

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FName GetRarityName() const
	{
		return RarityName;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FLinearColor GetRarityColor() const
	{
		return RarityColor;
	}
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetRarityName(const FName& Name);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetRarityColor(const FLinearColor& Color);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true))
	FName RarityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true))
	FLinearColor RarityColor;
	
};
