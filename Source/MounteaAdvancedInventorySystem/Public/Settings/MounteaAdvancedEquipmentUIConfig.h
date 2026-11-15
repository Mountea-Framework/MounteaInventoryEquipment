// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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
#include "MounteaAdvancedEquipmentUIConfig.generated.h"

class UUserWidget;

/**
 * Selects widget classes for standalone and embedded equipment presentation.
 * Equipment definitions and equip/unequip rules remain in the gameplay equipment config.
 * Screen, panel and slot interface constraints will follow their dedicated widget contracts.
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Equipment UI Settings Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining equipment UI."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedEquipmentUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Optional standalone equipment screen. Leave unset when equipment is embedded in the inventory screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryEquipmentWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentScreenWidgetClass;

	/** Collection host for equipment targets and their selection state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment",
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentPanelWidgetClass;

	/** Individual named equipment target, including its empty and equipped states. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment",
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentSlotWidgetClass;

	/** Optional collection of compatible inventory items for the selected equipment target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Candidates",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentCandidatesWidgetClass;

	/** Optional detail panel for the selected equipped item or candidate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemPanelWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentItemPanelWidgetClass;
};
