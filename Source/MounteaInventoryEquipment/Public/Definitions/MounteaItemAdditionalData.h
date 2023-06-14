// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "MounteaItemAdditionalData.generated.h"

#define LOCTEXT_NAMESPACE "MounteaItemAdditionalData"

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Additional Data")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaItemAdditionalData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip", meta=(ShowOnlyInnerProperties, MultiLine=true))
	FText ItemShortDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip", meta=(ShowOnlyInnerProperties, MultiLine=true))
	FText ItemLongDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip", meta=(ShowOnlyInnerProperties))
	UTexture2D* ItemThumbnail = nullptr;
};

#undef LOCTEXT_NAMESPACE