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

	UInventoryKeyAction(const FText& NewActionName, const FGuid& NewActionGuid, const TSet<FInventoryKeyActionData>& NewPlatformBasedMapping) :
		ActionName(NewActionName),
		ActionGuid(NewActionGuid),
		PlatformBasedMapping(NewPlatformBasedMapping)
	{};
	
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Inventory")
	bool ExecuteAction();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetActionName() const { return ActionName; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FGuid GetActionGuid() const {return ActionGuid; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSet<FInventoryKeyActionData> GetPlatformBasedMapping() const {return PlatformBasedMapping; };
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=True))
	FText ActionName = LOCTEXT("InventoryActionName", "Use");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid ActionGuid = FGuid::NewGuid();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=True, ShowOnlyInnerProperties, TitleProperty="PlatformName"))
	TSet<FInventoryKeyActionData> PlatformBasedMapping;
};

#undef LOCTEXT_NAMESPACE