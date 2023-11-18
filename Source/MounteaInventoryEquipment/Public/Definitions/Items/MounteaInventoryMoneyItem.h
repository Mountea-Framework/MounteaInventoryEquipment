// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Definitions/MounteaInventoryItem.h"

#include "MounteaInventoryMoneyItem.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemMoney"

/**
 * Inventory Item Money Data Asset
 * 
 * Base class for inventory item templates.
 *
 * To provide a static definition specifically for monetary value in the game.
 * It's a specific identifier for money within the inventory system but doesn't need any specific properties beyond what UBaseItem offers.
 *
 * @see UDataAsset
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Money
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea", DisplayName="Inventory Item (Money)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemMoney : public UMounteaInventoryItemBase
{
	GENERATED_BODY()
};

#undef LOCTEXT_NAMESPACE