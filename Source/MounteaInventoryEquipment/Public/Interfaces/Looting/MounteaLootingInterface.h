// All rights reserved Dominik Morse 2023.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Helpers/MounteaItemHelpers.h"
#include "UObject/Interface.h"
#include "MounteaLootingInterface.generated.h"

struct FItemItem;

class IMounteaInventoryInterface;
class UMounteaInstancedItem;

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
	void SetSourceInventory(const TScriptInterface<IMounteaInventoryInterface>& NewSource);
	virtual void SetSourceInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewSource) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetTargetInventory() const;
	virtual TScriptInterface<IMounteaInventoryInterface> GetTargetInventory_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetTargetInventory(const TScriptInterface<IMounteaInventoryInterface>& NewTarget);
	virtual void SetTargetInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewTarget) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInstancedItem*> GetLootableItems() const;
	virtual TArray<UMounteaInstancedItem*> GetLootableItems_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanLootItem(UMounteaInstancedItem* Item, const int32& Quantity = 1) const;
	virtual bool CanLootItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult LootItem(const FItemTransfer& Item);
	virtual FInventoryUpdateResult LootItem_Implementation(const FItemTransfer& Item) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<FInventoryUpdateResult> LootAllItems(const TArray<FItemTransfer>& Items);
	virtual TArray<FInventoryUpdateResult> LootAllItems_Implementation(const TArray<FItemTransfer>& Items) = 0;

	/**
	 * Retrieves the actor to which this inventory belongs.
	 * @return The actor that owns this Inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	AActor* GetOwningActor() const;
	virtual AActor* GetOwningActor_Implementation() const = 0;
	
	/**
	 * Checks if the Inventory has authority within the networked game.
	 * @return True if the component has authority, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool DoesHaveAuthority() const;
	virtual bool DoesHaveAuthority_Implementation() const = 0;
};
