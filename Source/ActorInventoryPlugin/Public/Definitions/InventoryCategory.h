// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryCategory.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryCategory : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	FName CategoryName;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 CategoryID;
};
