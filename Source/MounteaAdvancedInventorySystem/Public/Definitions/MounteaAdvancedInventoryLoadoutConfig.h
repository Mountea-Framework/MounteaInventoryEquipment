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
#include "MounteaAdvancedInventoryLoadoutConfig.generated.h"

class UMounteaAdvancedInventoryLoadoutItem;

/**
 * UMounteaAdvancedInventoryLoadoutConfig stores a predefined set of loadout entries as a data asset.
 * It defines which items should be granted by a loadout component, including per-item setup such as
 * quantity, durability, and optional equipment slot preferences.
 *
 * @see [Loadout System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/LoadoutSystem)
 * @see UMounteaAdvancedInventoryLoadoutComponent
 * @see UMounteaAdvancedInventoryLoadoutItem
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, 
	meta=(DisplayName = "Mountea Loadout Configuration"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryLoadoutConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Ordered list of item entries that make up this loadout configuration.
	 *
	 * Each entry describes one item to add when the loadout is executed. Entries are instanced inline
	 * for direct editing in the asset and are consumed by UMounteaAdvancedInventoryLoadoutComponent
	 * during LoadLoadout.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		Instanced,
		meta=(NoResetToDefault),
		meta=(TitleProperty="item: {DisplayName} | slot: {EquipmentSlot}"),
		meta=(ShowInnerProperties, ShowOnlyInnerProperties),
		meta=(FullyExpand=true))
	TArray<TObjectPtr<UMounteaAdvancedInventoryLoadoutItem>> Items;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
