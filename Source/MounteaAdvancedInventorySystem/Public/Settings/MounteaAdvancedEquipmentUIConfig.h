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
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Equipment UI Settings Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining equipment UI."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedEquipmentUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Optional standalone equipment screen; leave unset for embedded equipment presentation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipment",
		DisplayName="Equipment Screen",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryEquipmentWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentScreenWidgetClass;

	/** Optional container for equipment category filters. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Categories",
		DisplayName="Equipment Categories Wrapper",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoriesWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentCategoriesWrapperWidgetClass;

	/** Individual equipment category entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Categories",
		DisplayName="Equipment Category",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentCategoryWidgetClass;

	/** Collection of compatible inventory items for the selected equipment target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items",
		DisplayName="Equipment Slots Wrapper",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentSlotsWrapperWidgetClass;

	/** Individual named equipment target, including its empty and equipped states. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items",
		DisplayName="Equipment Slot",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryEquipmentSlotsWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentSlotWidgetClass;

	/** Individual inventory item presented as an equipment candidate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items",
		DisplayName="Equipment Item",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentItemWidgetClass;

	/** Optional details panel for the selected equipped item or candidate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item",
		DisplayName="Equipment Panel",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemPanelWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentItemPanelWidgetClass;

	/** Optional tooltip for the hovered or selected equipment item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item",
		DisplayName="Equipment Tooltip",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryTooltipWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentTooltipWidgetClass;

	/** Container for the selected equipment item actions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Action",
		DisplayName="Equipment Actions Container",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionsContainerWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentActionsContainerWidgetClass;

	/** Individual equipment item action. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Action",
		DisplayName="Equipment Action",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> EquipmentActionWidgetClass;
};
