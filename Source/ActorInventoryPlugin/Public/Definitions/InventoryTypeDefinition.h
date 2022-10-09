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

	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Transaction Settings", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TMap<UInventoryTypeDefinition*, TSubclassOf<UInventoryTransaction>> TransactionsFrom;

	UPROPERTY(SaveGame, EditDefaultsOnly, Category="Inventory|Transaction Settings", NoClear, meta=(NoResetToDefault, AllowAbstract=false))
	TMap<UInventoryTypeDefinition*, TSubclassOf<UInventoryTransaction>> TransactionsTo;
};

#undef LOCTEXT_NAMESPACE