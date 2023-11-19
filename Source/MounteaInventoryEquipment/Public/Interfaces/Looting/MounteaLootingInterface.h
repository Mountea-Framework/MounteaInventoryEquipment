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

/*
* Interface for looting functionality.
* Implementing this interface allows actors to interact with and manage lootable items.
*/
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaLootingInterface
{
	GENERATED_BODY()

public:

	/**
	 * Retrieves the inventory component that serves as the source of lootable items.
	 * Implementing classes will define how this inventory is determined and accessed.
	 * @return Interface reference to the Source Inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetSourceInventory() const;
	virtual TScriptInterface<IMounteaInventoryInterface> GetSourceInventory_Implementation() const = 0;

	/**
	 * Sets a new inventory component as the source for looting operations.
	 * @param NewSource The new target inventory to set.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetSourceInventory(const TScriptInterface<IMounteaInventoryInterface>& NewSource);
	virtual void SetSourceInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewSource) = 0;

	/**
	 * Retrieves the inventory component that serves as the target of lootable items.
	 * Implementing classes will define how this inventory is determined and accessed.
	 * @return Interface reference to the Target Inventory.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetTargetInventory() const;
	virtual TScriptInterface<IMounteaInventoryInterface> GetTargetInventory_Implementation() const = 0;

	/**
	 * Sets a new inventory component as the target for looting operations.
	 * @param NewTarget The new target inventory to set.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SetTargetInventory(const TScriptInterface<IMounteaInventoryInterface>& NewTarget);
	virtual void SetTargetInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewTarget) = 0;

	/**
	 * Gets a list of items that are available for looting.
	 * @return An array of lootable instanced items.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInstancedItem*> GetLootableItems() const;
	virtual TArray<UMounteaInstancedItem*> GetLootableItems_Implementation() const = 0;

	/**
	 * Determines if a specific item can be looted from the source inventory.
	 * @param Item The item to check for looting feasibility.
	 * @param Quantity The quantity of the item to check.
	 * @return True if the item can be looted, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanLootItem(UMounteaInstancedItem* Item, const int32& Quantity = 1) const;
	virtual bool CanLootItem_Implementation(UMounteaInstancedItem* Item, const int32& Quantity = 1) const = 0;

	/**
	* Attempts to loot a specified item from the source inventory.
	* @param Item The item and quantity to be looted.
	* @return A structure detailing the result of the loot attempt.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FInventoryUpdateResult LootItem(const FItemTransfer& Item);
	virtual FInventoryUpdateResult LootItem_Implementation(const FItemTransfer& Item) = 0;

	/**
	 * Attempts to loot multiple items from the source inventory.
	 * @param Items An array of items and their quantities to be looted.
	 * @return An array of structures detailing the results of each loot attempt.
	 */
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
