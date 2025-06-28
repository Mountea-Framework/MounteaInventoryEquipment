// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedEquipmentSettingsConfig.generated.h"

/**
* Data Asset that manages configuration settings for Mountea Advanced Equipment System.
* Defines Equipment Slots and other settings.
*
* @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Equipment-Settings-Config
*/
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Equipment Settings Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Equipment System defining equipment types, slots and other settings."))

class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedEquipmentSettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🧩 Slots",
		meta=(ForceInlineRow, NoResetToDefault, ShowOnlyInnerProperties))
	TMap<FName, FMounteaEquipmentSlotHeaderData> AllowedEquipmentSlots;

};
