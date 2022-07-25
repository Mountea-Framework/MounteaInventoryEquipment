// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryKeyAction.generated.h"

#define LOCTEXT_NAMESPACE "InventoryKeyAction"

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGIN_API UInventoryKeyAction : public UDataAsset
{
	GENERATED_BODY()

	UInventoryKeyAction();
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetActionName() const { return ActionName;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSet<FInventoryKeyActionData> GetPlatformBasedMapping() const {return PlatformBasedMapping; };
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=True))
	FText ActionName = LOCTEXT("InventoryActionName", "Use");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=True, ShowOnlyInnerProperties, TitleProperty="PlatformName"))
	TSet<FInventoryKeyActionData> PlatformBasedMapping;
};

#undef LOCTEXT_NAMESPACE