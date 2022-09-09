// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/InventoryItemSlotUse.h"
#include "Widgets/InventoryItemSlotUse.h"
#include "InventoryTypeDefinition.generated.h"

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
	
protected:

	/**
	 * Name of the Inventory Type
	 */
	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(NoResetToDefault=true))
	FText InventoryTypeName = LOCTEXT("InventoryType", "Player Inventory");
	
	/**
	 * Unique Guid
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid InventoryTypeGuid = FGuid::NewGuid();
};

#undef LOCTEXT_NAMESPACE