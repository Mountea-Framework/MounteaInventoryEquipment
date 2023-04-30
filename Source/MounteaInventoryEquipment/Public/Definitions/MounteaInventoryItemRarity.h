// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryItemRarity.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemRarity"

/**
 * Mountea Inventory Rarity
 *
 * Abstract Base Class
 *
 * Defines Rarity.
 * Is Assigned to Inventory Items to sort them to Rarities.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemRarity : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="1. Required")
	FText RarityName = LOCTEXT("MounteaInventoryItemRarity_Name", "Default");

	UPROPERTY(EditDefaultsOnly, Category="1. Required", meta=(HideAlphaChannel))
	FLinearColor RarityColor = FLinearColor::Gray;
};

#undef LOCTEXT_NAMESPACE