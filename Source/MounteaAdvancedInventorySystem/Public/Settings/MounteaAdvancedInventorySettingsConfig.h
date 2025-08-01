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
#include "Definitions/MounteaAdvancedInventoryNotification.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventorySettingsConfig.generated.h"

class UMounteaAdvancedInventoryThemeConfig;
class UCommonActivatableWidget;

/**
 * UMounteaAdvancedInventorySettingsConfig is a data asset managing comprehensive inventory system configuration.
 * Settings config defines inventory types, item rarities, categories, UI classes, notification systems,
 * and visual parameters for complete inventory system customization and behavior control.
 *
 * @see [Settings Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see FInventoryTypeConfig
 * @see FInventoryRarity
 * @see FInventoryCategory
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining inventory types, rarities and categories."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventorySettingsConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventorySettingsConfig();

public:

	// --- Types ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="⚜ Types", meta=(ForceInlineRow, NoResetToDefault))
	TMap<EInventoryType, FInventoryTypeConfig> AllowedInventoryTypes;

	// --- Rarities ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="💎 Rarities", meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryRarity> AllowedRarities;

	// --- Categories ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="📚 Categories", meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FInventoryCategory> AllowedCategories;

	// --- Limits ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="♾ Limits", meta=(NoResetToDefault))
	float InventoryBaseWeightLimit = 150.f;

	// --- Templates ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🏗 Templates", meta=(NoResetToDefault))
	TMap<FString,TSoftObjectPtr<UPrimaryDataAsset>> TemplatesConfig;

	// --- User Interface ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Theme")
	TSoftObjectPtr<UMounteaAdvancedInventoryThemeConfig> BaseTheme;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Theme", meta=(UIMin=0.f,ClampMin=0.f))
	FVector4 BaseBorderRadius = FVector4(6.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Visual", meta=(UIMin=0.f,ClampMin=0.f))
	float BaseBorderWidth = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Visual")
	float ItemSlotPadding = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Visual")
	FIntPoint InventoryGridDimensions = FIntPoint(5, 8);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Visual")
	FVector2D ItemSlotSize = FVector2D(128.0f, 128.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Visual", meta=(UIMin=0.1f,UIMax=10.f,ClampMin=0.1f,ClampMax=10.f))
	float ItemSlotAspectRatio = 1.f;

	/**
	 * Determines if stackable items should always automatically stack together when added to the inventory.
	 * If set to true, stackable items will occupy the same inventory slot until the maximum stack size is reached.
	 * Then other non-filled slot will be found and so on, until no empty slots are available and/or input quantity if reached.
	 * Improves inventory organization by reducing the number of individual item slots occupied.
	 *
	 * ⚠ Can result in performance impact on lower-end machines, especially with huge amounts of items!
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Settings", DisplayName="⚠ Always Stack Stackable Items")
	uint8 bAlwaysStackStackableItems : 1;

	/** Determines if the inventory system allows drag-and-drop operations for items. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "✨ UserInterface|Settings")
	uint8 bAllowDragAndDrop : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Config", meta=(NoResetToDefault))
	TSet<FString> WidgetCommands;

	// --- User Interface: Main UI

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Wrapper",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventorySystemBaseWidgetInterface"))
	TSoftClassPtr<UCommonActivatableWidget> UserInterfaceWrapperClass;

	// --- User Interface: Inventory
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryWidgetClass;
	
	// --- User Interface: Items

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Items",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemSlotsWrapperWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Items",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemSlotWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemSlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Items",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Items",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemsGridWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemsGridWidgetClass;

	// --- User Interface: Item Thumbnail
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Tooltips",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryTooltipWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemTooltipWidgetClass;

	// --- user Interface: Item Actions
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Item Actions",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionsContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemActionsContainerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Item Actions",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemActionWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemActionsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Item Actions",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryActionQuantityProviderWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemActionsQuantityProviderWidgetClass;
	
	// --- User Interface: Item Panel

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Item Panel",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryItemPanelWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryItemPanelWidgetClass;

	// --- User Interface: Categories

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Categories",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoriesWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryCategoriesWrapperWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Inventory|Categories",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryCategoryWidgetInterface"))
	TSoftClassPtr<UUserWidget> InventoryCategoryWidgetClass;
	
	// --- Notifications ------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Notifications",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetContainerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Classes|Notifications",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🔔 Notifications|Style", meta=(ForceInlineRow))
	TMap<EInventoryNotificationCategory, FInventoryNotificationStyle> NotificationCategoryStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🔔 Notifications|Config", meta=(NoResetToDefault))
	TSet<FString> NotificationTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="🔔 Notifications|Config", meta=(ForceInlineRow), meta=(GetKeyOptions="GetNotificationTypes"))
	TMap<FString, FInventoryNotificationConfig> NotificationConfigs;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "🔔 Notifications|User Interface")
	FVector2D NotificationSizeOverride = FVector2D(400.0f, 50.0f);
	
protected:

	void ValidateInventoryTypes();
	static FInventoryTypeConfig GetDefaultConfigForType(const EInventoryType Type);

	static void SetupPlayerConfig(FInventoryTypeConfig& Config);
	static void SetupNPCConfig(FInventoryTypeConfig& Config);
	static void SetupStorageConfig(FInventoryTypeConfig& Config);
	static void SetupMerchantConfig(FInventoryTypeConfig& Config);
	static void SetupLootConfig(FInventoryTypeConfig& Config);
	static void SetupSpecializedConfig(FInventoryTypeConfig& Config);

	void SetupWidgetCommands();

	UFUNCTION()
	TArray<FString> GetNotificationTypes() const;

#if WITH_EDITOR
public:
	/** Sets default values. Default values can be edited. If requested then Default values will override all `Categories` and `Rarities`! */
	UFUNCTION(CallInEditor, Category="⚙ Default Values")
	void SetDefaultValues();

	UFUNCTION(CallInEditor, Category="⚙ Default Values")
	void SetDefaultNotificationConfig();
protected:
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
