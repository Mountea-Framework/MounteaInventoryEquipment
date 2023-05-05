// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaInventoryInterface.generated.h"

class UMounteaInventoryItem_Base;

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMounteaInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Mountea Inventory Interface.
 *
 * Provides functions to manage Inventory, for example:
 * * Provide info about Inventory content
 * * Add new Items
 * * Remove Items
 * * Request Item Actions
 *
 * Inventory is player-specific and thus should be attached to Player Controller.
 * If player inventory is tied to Player pawn (if player dies, his loot stays on his corpse), then use Player Pawn instead.
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool LoadInventoryFromDataTable(const class UDataTable* SourceTable);
	virtual bool LoadInventoryFromDataTable_Implementation(const class UDataTable* SourceTable) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void SaveInventory();
	virtual void SaveInventory_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaInventoryItem_Base* GetItem(const FGuid& ItemGUID);
	virtual UMounteaInventoryItem_Base* GetItem_Implementation(const FGuid& ItemGUID) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	TArray<UMounteaInventoryItem_Base*> GetItems(const FItemRetrievalFilter OptionalFilter);
	virtual TArray<UMounteaInventoryItem_Base*> GetItems_Implementation(const FItemRetrievalFilter OptionalFilter) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItem(UMounteaInventoryItem_Base* NewItem);
	virtual bool AddItem_Implementation(UMounteaInventoryItem_Base* NewItem) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool AddItems(TArray<UMounteaInventoryItem_Base*>& NewItems);
	virtual bool AddItems_Implementation(TArray<UMounteaInventoryItem_Base*>& NewItems) = 0;
};
