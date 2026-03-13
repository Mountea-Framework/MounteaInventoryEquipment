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
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryLoadout.generated.h"

class UMounteaAdvancedInventoryLoadoutItem;

/**
 * Represents an advanced inventory loadout for the Mountea inventory system.
 *
 * This class functions as a data asset that encapsulates a collection of inventory items. It can be used
 * to define predefined loadouts for the Mountea inventory system, enabling easier management of item sets.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, meta=(DisplayName = "Mountea Loadout"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryLoadout : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Represents a collection of advanced inventory loadout items used in the Mountea inventory system.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		Instanced,
		meta=(NoResetToDefault),
		meta=(TitleProperty="item: {DisplayName} | slot: {EquipmentSlot}"),
		meta=(ShowInnerProperties, ShowOnlyInnerProperties),
		meta=(FullyExpand=true))
	TArray<TObjectPtr<UMounteaAdvancedInventoryLoadoutItem>> Items;
};
