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

struct FMounteaWidgetInputActionMapping;

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
	
	// --- UI Inputs
	
	/** Discrete UI actions (Close, Confirm, ContextMenu, NextTab, PreviousTab, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|🎮 Inputs")
	TArray<FMounteaWidgetInputActionMapping> UIActionMappings;
	

	// --- Wrapper
	
	/** Widget class used as the main user interface wrapper (root HUD panel, container for other widgets, 
	 * like Inventory, Equipment, Crafting menu etc.). This Wrapper should be used for other UI systems to
	 * avoid Z-oder fighting.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🧩 Wrapper",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaInventorySystemWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> UserInterfaceWrapperClass;
	
	// --- Inventory
	
	/** Widget class used to display the inventory itself (slots, items, navigation inside the inventory). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🎒 Inventory",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryWidgetClass;
	
	// ---- Notifications
	
	/** Widget class that acts as a container for all inventory notifications (list / stack of notification widgets). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🔔 Notifications",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetContainerClass;
	
	/** Widget class used for a single inventory notification entry (one message / card in the notification container). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🔔 Notifications",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationWidgetClass;
	
	/** Material used for rendering individual notification cards (background, styling, effects). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🔔 Notifications|💳 Notification Card")
	TSoftObjectPtr<UMaterialInterface> NotificationCardMaterial = nullptr;
	
	/** Default size (in Slate units) of the notification card widget when rendered on screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🔔 Notifications|💳 Notification Card")
	FVector2f NotificationCardSize = FVector2f(150.f, 60.f);
	
	// ---- Categories
	
	/** Widget class used as a container for all inventory categories (tabs, filters, or category list). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🗂 Categories",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoriesWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> CategoriesContainerWidgetClass;
	
	/** Widget class representing a single inventory category entry (one category button / tab). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🗂 Categories",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> CategoryWidgetClass;
	
	// ---- Items
	
	/** Widget class used as a container for inventory item widgets (grid, list, or panel of item slots). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|📦 Items",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> ItemsContainerWidgetClass;
	
	/** Widget class representing a single inventory item (slot visual, icon, quantity, tooltip, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|📦 Items",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface"))
	TSoftClassPtr<UUserWidget> ItemWidgetClass;
	
	// ---- Item
	
	/** Widget class used to detailed Item Information. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🏺 Item",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemPanelWidgetInterface"))
	TSoftClassPtr<UUserWidget> ItemPanelWidgetClass;
	
	/** Widget class used to display Tooltip information of currently selected/hovered item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🏺 Item",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryTooltipWidgetInterface"))
	TSoftClassPtr<UUserWidget> ItemTooltipWidgetClass;
	
	// ---- Item Actions
	
	/** Widget class used display list of available Item Actions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🎛 Item Action",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionsContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> ItemActionsContainerWidgetClass;
	
	/** Widget class used to display individual Item Actions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🎛 Item Action",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionWidgetInterface"))
	TSoftClassPtr<UUserWidget> ItemActionWidgetClass;
	
	// ---- Fonts
	
	/** Default font settings used across the inventory user interface (labels, counters, titles). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|🔤 Font")
	FSlateFontInfo DefaultFont;
	
	// ---- Settings
	
	/** Defines list of available Widget Commands. Those are available using custom SwithOnWidgetCommand Node.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|🎚 Config", 
		meta=(NoResetToDefault))
	TSet<FString> WidgetCommands;
	
	/**
	 * Determines if stackable items should always automatically stack together when added to the inventory.
	 * If set to true, stackable items will occupy the same inventory slot until the maximum stack size is reached.
	 * Then other non-filled slot will be found and so on, until no empty slots are available and/or input quantity if reached.
	 * Improves inventory organization by reducing the number of individual item slots occupied.
	 *
	 * ⚠ Can result in performance impact on lower-end machines, especially with huge amounts of items!
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|⚙ Settings", 
		DisplayName="⚠ Always Stack Stackable Items")
	uint8 bAlwaysStackStackableItems : 1;

	/** Determines if the inventory system allows drag-and-drop operations for items. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|⚙ Settings")
	uint8 bAllowDragAndDrop : 1;
	
public:
	
	void SetupWidgetCommands();
	

#if WITH_EDITOR
protected:
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	
};

