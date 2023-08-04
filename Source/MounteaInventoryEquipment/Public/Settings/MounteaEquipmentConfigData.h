// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MounteaEquipmentConfigData.generated.h"

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Mountea Equipment Data")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentConfigData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TMap<FGameplayTag,FGameplayTagContainer> CompatibleInventoryFlags;

	UPROPERTY(EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, DisplayThumbnail=false))
	TMap<FString,FGameplayTag> EquipmentSlotIDs;
};
