// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Definitions/MounteaEquipmentBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedEquipmentSettingsConfig.generated.h"

/**
 * UMounteaAdvancedEquipmentSettingsConfig manages configuration for the Mountea Advanced Equipment System.
 * Equipment settings define allowed equipment slots with metadata for comprehensive equipment
 * management and attachment system configuration within the inventory framework.
 *
 * @see [Equipment Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see FMounteaEquipmentSlotHeaderData
 * @see UMounteaAdvancedInventorySettings
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
