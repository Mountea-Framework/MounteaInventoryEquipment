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
#include "UObject/Object.h"
#include "MounteaAdvancedInventoryLoadoutItem.generated.h"

class UMounteaInventoryItemTemplate;

/**
 * UMounteaAdvancedInventoryLoadoutItem defines one entry inside a loadout configuration.
 * It describes which item template should be granted, how much quantity/durability to apply,
 * and whether the item should be equipped automatically (optionally into a specific slot).
 *
 * @see [Loadout System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/LoadoutSystem)
 * @see UMounteaAdvancedInventoryLoadoutConfig
 * @see UMounteaAdvancedInventoryLoadoutComponent
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea"),
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Inventory Loadout Item"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryLoadoutItem : public UObject
{
	GENERATED_BODY()
	
public:
	
	UMounteaAdvancedInventoryLoadoutItem();
	
public:
	
	/**
	 * Item template to spawn into inventory when this loadout entry is processed.
	 *
	 * This template drives default quantity, durability, and possible equipment targeting.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	TObjectPtr<UMounteaInventoryItemTemplate> ItemTemplate = nullptr;
	
	/**
	 * Inclusive random quantity range used when random quantity is enabled.
	 * The range is clamped based on Item Template's quantity settings.
	 *
	 * This range is hidden unless bUseRandomQuantity is true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(EditCondition="bUseRandomQuantity", EditConditionHides),
		meta=(NoResetToDefault))
	FIntPoint RandomRange = FIntPoint(1, 100);
	
	/**
	 * Base quantity granted for this loadout entry.
	 *
	 * When an item template is assigned in the editor, this value is clamped to the template's
	 * supported maximum quantity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	int32 BaseQuantity = 1;
	
	/**
	 * Base durability value applied to the granted item.
	 *
	 * When an item template is assigned in the editor, this value is clamped to the template's
	 * supported maximum durability.
	 * For non-durable templates this is reset to a sentinel value in editor validation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	int32 BaseDurability = 1;
	
	/**
	 * If true, quantity should be generated from RandomRange instead of using BaseQuantity directly.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	uint8 bUseRandomQuantity : 1;
	
	/**
	 * If true, the loadout system attempts to equip this item after adding it to inventory.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	uint8 bAutomaticallyEquip : 1;
	
	/**
	 * Preferred equipment slot for automatic equip operations.
	 *
	 * Available options are sourced from the equipment settings configuration.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(GetOptions="GetAvailableSlotNames"),
		meta=(NoResetToDefault))
	FName EquipmentSlot = NAME_None;
	
#if WITH_EDITORONLY_DATA
	/**
	 * Editor-only display label used to make loadout entries easier to identify in details panels.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	FName DisplayName = NAME_None;
#endif
	
protected:
	
	UFUNCTION()
	static TArray<FName> GetAvailableSlotNames();
	
	void ApplyDefaults();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
