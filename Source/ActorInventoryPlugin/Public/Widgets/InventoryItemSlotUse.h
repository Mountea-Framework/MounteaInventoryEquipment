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
	EST_MERGE_SameInv	UMETA(DisplayName="Merge", Tooltip="Merge in Same Inventory"),
	EST_SPLIT_SameInv	UMETA(DisplayName="Split", Tooltip="Split in Same Inventory"),
	EST_Merge_DiffInv	UMETA(DisplayName="Merge", Tooltip="Merge from Different Inventory"),
	EST_SPLIT_DiffInv	UMETA(DisplayName="Split", Tooltip="Split into Different Inventory"),
	EST_BuySell			UMETA(DisplayName="Buy or Sell"),
	EST_Loot			UMETA(DisplayName="Loot"),

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
	void Init(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot, UInventoryTypeDefinition* NewUseType);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget);

	/**
	 * @brief Validation function which returns whether the Slot is Valid or not. Validation is different for Split/Merge and Use/Drop.
	 * @return True if Use/Drop and OriginSlot is valid, True if Merge/Split and OriginSlot and TargetSlot are valid, default false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool IsValid() const
	{
		/*
		switch (UseType)
		{
			case EUseType::EST_Use:
			case EUseType::EST_Drop:
				return OriginSlot != nullptr;
			case EUseType::EST_MERGE_SameInv:
			case EUseType::EST_SPLIT_SameInv: 
			case EUseType::EST_Merge_DiffInv:
			case EUseType::EST_SPLIT_DiffInv:
				return OriginSlot != nullptr && TargetSlot != nullptr;
			case EUseType::Default:
			default:
				return false;
		}
		*/
		return true;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetUseType(UInventoryTypeDefinition* NewUseType);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	FString GetUseTypeText() const;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Type", meta=(NoResetToDefault=true, BlueprintBaseOnly=true))
	UInventoryTypeDefinition* InventoryUseType = nullptr;

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