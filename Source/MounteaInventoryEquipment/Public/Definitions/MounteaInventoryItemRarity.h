// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
UCLASS( BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemRarity : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="1. Required")
	FGameplayTag RarityGameplayTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="1. Required")
	FText RarityName = LOCTEXT("MounteaInventoryItemRarity_Name", "Default");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="1. Required", meta=(HideAlphaChannel))
	FLinearColor RarityColor = FLinearColor::Gray;
};

#undef LOCTEXT_NAMESPACE