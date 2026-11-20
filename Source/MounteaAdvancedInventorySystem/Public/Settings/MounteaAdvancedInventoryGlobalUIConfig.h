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
#include "Definitions/MounteaAdvancedInventoryGlobalUIDataTypes.h"
#include "Definitions/MounteaInventoryBaseUIDataTypes.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryGlobalUIConfig.generated.h"

class UMounteaAdvancedInventoryThemeConfig;
class UMaterialInterface;
class UDataTable;
class UUserWidget;

/**
 * UMounteaAdvancedInventoryGlobalUIConfig manages global UI configuration for
 * the Mountea Advanced Inventory & Equipment System.
 *
 * @see [UI Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see UMounteaAdvancedInventorySettings
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Global UI Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory & Equipment System UI."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryGlobalUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventoryGlobalUIConfig();

public:

	/** Default theme shared by wrapper chrome and screens without an override. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme")
	TSoftObjectPtr<UMounteaAdvancedInventoryThemeConfig> BaseTheme;

	/** Designer prefix rules shared by runtime application and editor auditing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme")
	TArray<FMounteaWidgetPrefixGroup> WidgetPrefixGroups;

	// --- UI Inputs

	/** Discrete UI actions (Close, Confirm, ContextMenu, NextTab, PreviousTab, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs",
		meta=(ShowOnlyInnerProperties),
		meta=(TitleProperty="ActionTag"),
		meta=(NoResetToDefault),
		DisplayName="Action Mappings for UI")
	TArray<FMounteaWidgetInputActionMapping> UIActionMappings;

	/** Unified deadzone used for Analog and Wheel inputs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs|Config",
		meta=(NoResetToDefault),
		meta=(UIMin=0.f, ClampMin=0.f))
	float InputDeadzone = 0.1f;

	// --- Wrapper

	/** Widget class used as the main user interface wrapper (root HUD panel, container for other widgets,
	 * like Inventory, Equipment, Crafting menu etc.). This Wrapper should be used for other UI systems to
	 * avoid Z-oder fighting.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wrapper",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaInventorySystemWrapperWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> UserInterfaceWrapperClass;

	// ---- Notifications

	/** Widget class that acts as a container for all inventory notifications (list / stack of notification widgets). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationContainerWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> NotificationWidgetContainerClass;

	/** Widget class used for a single inventory notification entry (one message / card in the notification container). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> NotificationWidgetClass;

	/** Material used for rendering individual notification cards (background, styling, effects). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications|Notification Card")
	TSoftObjectPtr<UMaterialInterface> NotificationCardMaterial = nullptr;

	/** Default size (in Slate units) of the notification card widget when rendered on screen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications|Notification Card")
	FVector2f NotificationCardSize = FVector2f(150.f, 60.f);

	// ---- Chrome

	/**
	 * Widget class used to display the header toolbar.
	 *
	 * The assigned widget must implement the MounteaAdvancedInventoryHeaderToolbarWidgetInterface
	 * to provide header-level controls and toolbar behavior for the UI.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryHeaderToolbarWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> HeaderWidgetClass;

	/**
	 * Widget class used to display the footer toolbar.
	 *
	 * The assigned widget must implement the MounteaAdvancedInventoryFooterToolbarWidgetInterface
	 * to provide footer-level controls and toolbar behavior for the UI.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notifications",
		meta = (MustImplement = "/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryFooterToolbarWidgetInterface"),
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget>FooterWidgetClass;

	// ---- Fonts

	/** Default font settings used across the inventory user interface (labels, counters, titles). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Font",
		meta=(NoResetToDefault))
	FSlateFontInfo DefaultFont;

	// ---- Commands

	/** Defines list of available Widget Commands. Those are available using custom ProcessWidgetCommand Node.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config & Settings",
		meta=(NoResetToDefault))
	TSet<FString> WidgetCommands;

	// ---- Modals

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault))
	TSet<FString> ModalTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault),
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalWidgetInterface"))
	TSoftClassPtr<UUserWidget> ModalWindowWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault),
		meta=(ShowOnlyInnerProperties),
		meta=(GetKeyOptions="GetModalTypes"))
	TMap<FString,FMounteaModalDefinition> Modals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault),
		meta=(RequiredAssetDataTags="RowStructure=/Script/MounteaAdvancedInventorySystem.MounteaModalsConfig"))
	TSet<TSoftObjectPtr<UDataTable>> ModalsData;

protected:

	static const TArray<FString>& GetDefaultModalTypes();
	void ValidateModalTypes();
	void SetupWidgetCommands();

	UFUNCTION()
	TArray<FString> GetModalTypes() const;

#if WITH_EDITOR
public:
	UFUNCTION(CallInEditor, Category="Default Values",
		DisplayName="Setup Default Modals")
	void SetDefaultModalTypes();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
#if WITH_EDITOR
public:
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	void ValidateThemeRules(const UMounteaAdvancedInventoryThemeConfig* Theme, FDataValidationContext& Context) const;
#endif
};
