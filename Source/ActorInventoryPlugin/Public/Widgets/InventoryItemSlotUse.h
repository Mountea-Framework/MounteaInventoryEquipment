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
	EST_Use				UMETA(DispplayName="Use Item"),
	EST_Drop			UMETA(DispplayName="Drop Item"),
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
	void Init(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot, const EUseType& NewUseType);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget);

	/**
	 * @brief Validation function which returns whether the Slot is Valid or not. Validation is different for Split/Merge and Use/Drop.
	 * @return True if Use/Drop and OriginSlot is valid, True if Merge/Split and OriginSlot and TargetSlot are valid, default false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool IsValid() const
	{
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
