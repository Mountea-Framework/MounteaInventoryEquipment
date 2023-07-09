// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaItemAction.h"
#include "MounteaInventoryItemCategory.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemCategory"

/**
 * Mountea Inventory Category
 *
 * Defines Category.
 * Is Assigned to Inventory Items to sort them to Categories.
 */
UCLASS( BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemCategory : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required")
	FGameplayTagContainer CompatibleGameplayTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required")
	FText CategoryName = LOCTEXT("MounteaInventoryItemCategory_Name", "Default");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="1. Required", meta=(MultiLine))
	FText CategoryDescription = LOCTEXT("MounteaInventoryItemCategory_Name", "Default");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="2. Optional")
	UTexture2D* CategoryIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="2. Optional")
	TSet<FMounteaItemAction> CategoryActions;
};

#undef LOCTEXT_NAMESPACE