// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryItemCategory.generated.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemCategory"

/**
 * Mountea Inventory Category
 *
 * Abstract Base Class
 *
 * Defines Category.
 * Is Assigned to Inventory Items to sort them to Categories.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, ClassGroup="Mountea")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemCategory : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category="1. Required")
	FGameplayTagContainer CompatibleGameplayTags;
	
	UPROPERTY(EditDefaultsOnly, Category="1. Required")
	FText CategoryName = LOCTEXT("MounteaInventoryItemCategory_Name", "Default");

	UPROPERTY(EditDefaultsOnly, Category="1. Required", meta=(MultiLine))
	FText CategoryDescription = LOCTEXT("MounteaInventoryItemCategory_Name", "Default");
	
	UPROPERTY(EditDefaultsOnly, Category="2. Optional")
	UTexture* CategoryIcon = nullptr;
};

#undef LOCTEXT_NAMESPACE