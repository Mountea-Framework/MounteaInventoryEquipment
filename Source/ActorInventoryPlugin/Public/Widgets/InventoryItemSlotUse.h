// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlotUse.generated.h"

class UInventoryItemSlot;
class UInventoryWidget;
class UInventoryTypeDefinition;

UENUM(BlueprintType)
enum class EUseType : uint8
{
	EST_Use				UMETA(DispplayName="Use Item", Tooltip="Using an Item"),
	EST_Drop			UMETA(DispplayName="Drop Item", Tooltip="Drop an Item"),
	EST_Trade			UMETA(DisplayName="Trade",	Tooltip="Buy or Sell"),
	EST_Loot			UMETA(DisplayName="Loot",	Tooltip = "Loot from other Inventory"),
	
	EST_Merge			UMETA(DisplayName="Merge",	Tooltip="Merge Item"),
	EST_Split			UMETA(DisplayName="Split",	Tooltip="Split Item"),

	Default				UMETA(Hidden)
};

#define LOCTEXT_NAMESPACE "Inventory Use"

/**
 * 
 */
UCLASS(Abstract)
class ACTORINVENTORYPLUGIN_API UInventoryItemSlotUse : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	void Init(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot, EUseType NewUseType);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget);

	/**
	 * Validation function which returns whether the Slot is Valid or not. Validation is different for Split/Merge and Use/Drop.
	 * @return True if Use/Drop and OriginSlot is valid, True if Merge/Split and OriginSlot and TargetSlot are valid, default false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Inventory")
	bool IsValid() const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetUseType(EUseType NewUseType);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	FString GetUseTypeText() const;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Type", meta=(NoResetToDefault=true, BlueprintBaseOnly=true))
	EUseType InventoryUseType;

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

#undef LOCTEXT_NAMESPACE