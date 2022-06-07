// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemRarity.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryItemRarity : public UObject
{
	GENERATED_BODY()

	UInventoryItemRarity();

#pragma region GETTERS

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

#pragma endregion GETTERS

#pragma region SETTERS

public:
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetRarityName(const FName& Name);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetRarityColor(const FLinearColor& Color);

#pragma endregion SETTERS

#pragma region VARIABLES
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	FName RarityName;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	FLinearColor RarityColor;

#pragma endregion VARIABLES
};
