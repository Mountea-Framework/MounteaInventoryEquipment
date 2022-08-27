// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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

protected:

	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(NoResetToDefault=true))
	FText InventoryTypeName = LOCTEXT("InventoryType", "Player Inventory");
};

#undef LOCTEXT_NAMESPACE