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
#include "MounteaAdvancedInventoryUIConfig.generated.h"

/**
 * UMounteaAdvancedInventoryUIConfig is a data asset managing comprehensive inventory system UI configuration.
 * Settings config defines visual parameters for complete inventory system customization and behavior control.
 *
 * @see [Settings Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings UI Config", 
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining visual components."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UMounteaAdvancedInventoryUIConfig();
	
public:
	
	// --- Inventory
	
	/** Widget class used to display the inventory itself (slots, items, navigation inside the inventory). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> InventoryWidgetClass;
	
	// ---- Categories
	
	/** Widget class used as a container for all inventory categories (tabs, filters, or category list). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Categories",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoriesWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CategoriesContainerWidgetClass;
	
	/** Widget class representing a single inventory category entry (one category button / tab). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Categories",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> CategoryWidgetClass;
	
	// ---- Items
	
	/** Widget class used as a container for inventory item widgets (grid, list, or panel of item slots). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> ItemsContainerWidgetClass;
	
	/** Widget class representing a single inventory item (slot visual, icon, quantity, tooltip, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> ItemWidgetClass;
	
	// ---- Item
	
	/** Widget class used to detailed Item Information. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemPanelWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> ItemPanelWidgetClass;
	
	/** Widget class used to display Tooltip information of currently selected/hovered item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryTooltipWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> ItemTooltipWidgetClass;
	
	// ---- Item Actions
	
	/** Widget class used display list of available Item Actions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Action",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionsContainerWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> ItemActionsContainerWidgetClass;
	
	/** Widget class used to display individual Item Actions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Action",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> ItemActionWidgetClass;
	
	// ---- Modal
	
	/** Widget class used to display Modal Window. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modals",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> InventoryModalWidgetClass;
		
	// ---- Settings
	
	/**
	 * Determines if stackable items should always automatically stack together when added to the inventory.
	 * If set to true, stackable items will occupy the same inventory slot until the maximum stack size is reached.
	 * Then other non-filled slot will be found and so on, until no empty slots are available and/or input quantity if reached.
	 * Improves inventory organization by reducing the number of individual item slots occupied.
	 *
	 * ⚠ Can result in performance impact on lower-end machines, especially with huge amounts of items!
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="⚙ Config & Settings", 
		DisplayName="⚠ Always Stack Stackable Items",
		meta=(NoResetToDefault))
	uint8 bAlwaysStackStackableItems : 1;

	/** Determines if the inventory system allows drag-and-drop operations for items. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="⚙ Config & Settings",
		meta=(NoResetToDefault))
	uint8 bAllowDragAndDrop : 1;
	
	/** Determines whether focus will be automatically passed to Active Widget or not. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="⚙ Config & Settings",
		meta=(NoResetToDefault))
	uint8 bAllowAutoFocus : 1;
	
};

