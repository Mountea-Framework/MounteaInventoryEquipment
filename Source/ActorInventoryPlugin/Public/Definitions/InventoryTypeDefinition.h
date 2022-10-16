// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/InventoryItemSlotUse.h"
#include "Widgets/InventoryItemSlotUse.h"
#include "InventoryTypeDefinition.generated.h"

class UInventoryTransaction;

#define LOCTEXT_NAMESPACE "InventoryTypeDefinition"

/**
 * Inventory Type Definition.
 * Defines Type of all Inventories.
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Type-Definition
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryTypeDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetInventoryTypeName() const { return InventoryTypeName; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FGuid GetInventoryTypeGuid() const {return InventoryTypeGuid; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE bool IsTransactional() const {return bIsTransactional; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryTransaction> GetTransactionFrom(const UInventoryTypeDefinition* SourceType) const
	{
		return *TransactionsFrom.Find(SourceType);
	};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TSubclassOf<UInventoryTransaction> GetTransactionTo(const UInventoryTypeDefinition* SourceType) const
	{
		return *TransactionsTo.Find(SourceType);
	};
	
protected:

	/**
	 * Name of the Inventory Type
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(NoResetToDefault=true))
	FText InventoryTypeName = LOCTEXT("InventoryType", "Player Inventory");

	/** Defines whether a transaction action (like payment) must be processed when interacting with this Inventory Type. */
	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(NoResetToDefault=true))
	uint8 bIsTransactional : 1;
	
	/**
	 * Unique Guid
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid InventoryTypeGuid = FGuid::NewGuid();

	/**
	 * A list which defines what Transaction Type is used per what Inventory Type.
	 * This list defines what Transaction is used when Item is coming From other Inventory Type to this.
	 * If left empty, no Transaction will be processed when Item is coming From other inventory.
	 *
	 * Example:
	 * Item from Shop is Dropped to Player Inventory. Player is Buying.
	 * This Inventory Type is Player Inventory.
	 * Other Inventory Type is Shop Inventory.
	 * Here you would create mapping Shop - Buying.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Transaction Settings", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TMap<UInventoryTypeDefinition*, TSubclassOf<UInventoryTransaction>> TransactionsFrom;

	/**
	 * A list which defines what Transaction Type is used per what Inventory Type.
	 * This list defines what Transaction is used when Item is coming From this Inventory Type to other.
	 * If left empty, no Transaction will be processed when Item is coming To other inventory.
	 *
	 * Example:
	 * Item from Player Inventory is Dropped to Shop. Player is Selling.
	 * This Inventory Type is Player Inventory.
	 * Other Inventory Type is Shop Inventory.
	 * Here you would create mapping Shop - Selling.
	 */
	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Transaction Settings", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TMap<UInventoryTypeDefinition*, TSubclassOf<UInventoryTransaction>> TransactionsTo;
};

#undef LOCTEXT_NAMESPACE