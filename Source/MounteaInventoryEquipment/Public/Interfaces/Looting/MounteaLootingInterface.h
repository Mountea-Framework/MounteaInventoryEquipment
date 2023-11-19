// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Helpers/MounteaItemHelpers.h"
#include "UObject/Interface.h"
#include "MounteaLootingInterface.generated.h"

struct FItemSlot;

class IMounteaInventoryInterface;

// This class does not need to be modified.
UINTERFACE()
class UMounteaLootingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaLootingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetSourceInventory() const;
	virtual TScriptInterface<IMounteaInventoryInterface> GetSourceInventory_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetTargetInventory() const;
	virtual TScriptInterface<IMounteaInventoryInterface> GetTargetInventory_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FItemSlot> GetLootableItems() const;
	virtual TArray<FItemSlot> GetLootableItems_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanLootItem(const FItemSlot& Slot) const;
	virtual bool CanLootItem_Implementation(const FItemSlot& Slot) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult LootItem(const FItemSlot& Slot);
	virtual FInventoryUpdateResult LootItem_Implementation(const FItemSlot& Slot) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FInventoryUpdateResult> LootAllItems(const FItemSlot& Slots);
	virtual TArray<FInventoryUpdateResult> LootAllItems_Implementation(const FItemSlot& Slots) = 0;
};
