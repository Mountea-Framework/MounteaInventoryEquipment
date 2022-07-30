// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlotUse.generated.h"

class UInventoryItemSlot;
class UInventoryWidget;

UENUM(BlueprintType)
enum class EUseType : uint8
{
	EST_Use				UMETA(DispplayName = "Use Item"),
	EST_Drop			UMETA(DispplayName = "Drop Item"),
	EST_MERGE_SameInv	UMETA(DisplayName="MERGE - Same Inventory"),
	EST_SPLIT_SameInv	UMETA(DisplayName="SPLIT - Same Inventory"),
	EST_Merge_DiffInv	UMETA(DisplayName="MERGE - Different Inventories"),
	EST_SPLIT_DiffInv	UMETA(DisplayName="SPLIT - Different Inventories"),

	Default				UMETA(Hidden)
};

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryItemSlotUse : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void Init(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool IsValid() const
	{
		return OriginSlot != nullptr && TargetSlot != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetUseType(const EUseType& NewUseType);
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	EUseType UseType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TArray<FKey> CancelKeys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TArray<FKey> ConfirmKeys;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryWidget* OwningWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryItemSlot* OriginSlot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ExposeOnSpawn=true, AllowAbstract=false))
	UInventoryItemSlot* TargetSlot = nullptr;
};
