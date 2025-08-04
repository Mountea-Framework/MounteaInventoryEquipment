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
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryUIStatics.generated.h"

struct FMounteaInventoryGridSlot;
struct FInventoryItemSearchParams;
struct FMounteaItemActionData;

class UWidget;
class UTextBlock;
class UMounteaInventoryItemAction;
class UMounteaAdvancedInventoryInterface;
class UMounteaAdvancedInventoryThemeConfig;
class UMounteaAdvancedInventoryInteractableObjectWidget;
class IMounteaAdvancedInventoryCategoryWidgetInterface;
class IMounteaInventorySystemBaseWidgetInterface;
struct FKey;

UENUM(BlueprintType)
enum class EMounteaThemeLevel : uint8
{
	Primary		UMETA(DisplayName = "Primary"),
	Secondary	UMETA(DisplayName = "Secondary"),
	Tertiary	UMETA(DisplayName = "Tertiary")
};

UENUM(BlueprintType)
enum class EMounteaThemeState : uint8
{
	Normal		UMETA(DisplayName = "Normal"),
	Hovered		UMETA(DisplayName = "Hovered"),
	Active		UMETA(DisplayName = "Active"),
	Disabled	UMETA(DisplayName = "Disabled")
};

UENUM(BlueprintType)
enum class EMounteaThemeType : uint8
{
	Text UMETA(DisplayName="Text"),
	Background UMETA(DisplayName="Background"),
	Default UMETA(DisplayName="Default")
};

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*************************************************************/
	/************************* TEMPLATES *************************/
	/*************************************************************/
public:
	template<typename ReturnType, typename Func, typename... Args>
	static ReturnType ExecuteIfImplements(UObject* Target, const TCHAR* FunctionName, Func Function, Args&&... args)
	{
		if (!IsValid(Target))
		{
			LOG_ERROR(TEXT("[%s] Invalid Target provided!"), FunctionName);
			if constexpr (!std::is_void_v<ReturnType>)
				return ReturnType{};
			else return;
		}

		if (Target->Implements<UMounteaAdvancedInventoryUIInterface>())
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				Function(Target, Forward<Args>(args)...);
				return;
			}
			return Function(Target, Forward<Args>(args)...);
		}

		LOG_ERROR(TEXT("[%s] Target does not implement 'MounteaAdvancedInventoryUIInterface'!"), FunctionName);
		if constexpr (!std::is_void_v<ReturnType>)
			return ReturnType{};
		else return;
	}

public:

	/*************************************************************/
	/*********************** INTERNAL ****************************/
	/*************************************************************/
	
	static APlayerController* FindPlayerController(AActor* Actor, int SearchDepth);
	static void SetOwningInventoryUIInternal(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI);
	
public:

	/*************************************************************/
	/******************* BLUEPRINTABLE ***************************/
	/*************************************************************/

	// --- Helpers  ------------------------------
#pragma region Helpers


	/**
	 * Checks whether the main UI is currently open.
	 *
	 * This function determines if the main UI associated with the given
	 * Mountea Advanced Inventory UI Interface target is opened and
	 * visible to the user by evaluating its visibility state.
	 *
	 * @param Target Reference to an object implementing the MounteaAdvancedInventoryUIInterface.
	 * @return True if the main UI is open and visible in any form (Visible, HitTestInvisible, or SelfHitTestInvisible);
	 *         False if it is collapsed, hidden, or the target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Getter"))
	static bool IsMainUIOpen(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Updates the source inventory grid slot with the data from the target inventory grid slot.
	 *
	 * @param SourceData The inventory grid slot that will be updated.
	 * @param TargetData The inventory grid slot providing the data to update the source slot.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|GridSlots", meta=(CustomTag="MounteaK2Setter"))
	static void SetGridSlotData(UPARAM(ref) FMounteaInventoryGridSlot& SourceData, const FMounteaInventoryGridSlot& TargetData);

	/**
	 * Stores an item identified by its ItemId into the specified inventory grid slot.
	 *
	 * This function assigns the provided ItemId to the input SourceData slot, marking it as occupied.
	 *
	 * @param SourceData Reference to the inventory grid slot where the item will be stored.
	 * @param ItemId The unique identifier of the item to be stored in the inventory grid slot.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item", meta=(CustomTag="MounteaK2Setter"))
	static void StoreItem(UPARAM(ref) FMounteaInventoryGridSlot& SourceData, const FGuid& ItemId);

	/**
	 * Resets the given inventory grid slot by clearing its data.
	 *
	 * @param SourceData Reference to the inventory grid slot that will be reset.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item", meta=(CustomTag="MounteaK2Setter"))
	static void ResetItem(UPARAM(ref) FMounteaInventoryGridSlot& SourceData);

	/**
	 * Converts the provided inventory grid slot data into a string representation.
	 *
	 * @param SourceData Reference to the inventory grid slot data to be converted.
	 * @return A string representation of the given inventory grid slot data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Grid Slot - ToString")
	static FString GridSlot_ToString(const FMounteaInventoryGridSlot& SourceData);

	/**
	 * Function to check if a specific input key event is allowed based on the provided input name.
	 * Checks global Inventory settings for input definitions and validates the key event against them.
	 * 
	 * @param MouseEvent Mouse Input event to validate input from
	 * @param InputName Name if of the input mapping to check against, eg. "IA_InputAction" etc.
	 * @return True if the input is allowed, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Mouse Event - Is Input Allowed")
	static bool MouseEvent_IsInputAllowed(const FPointerEvent& MouseEvent, const FName& InputName);

	/**
	 * Function to check if a specific input key event is allowed based on the provided input name.
	 * Checks global Inventory settings for input definitions and validates the key event against them.
	 * 
	 * @param InKeyEvent Input event to validate input from
	 * @param InputName Name if of the input mapping to check against, eg. "IA_InputAction" etc.
	 * @return True if the input is allowed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Input Event - Is Input Allowed")
	static bool KeyEvent_IsInputAllowed(const FKeyEvent& InKeyEvent, const FName& InputName);

	static bool IsInputAllowed(const FKey& InputKey, const FName& InputName);

	/**
	 * Finds an item in the inventory using the provided search parameters.
	 * This function retrieves the parent inventory from the target UI component
	 * and searches for the item based on the specified search parameters.
	 *
	 * @param Target The target UI component implementing MounteaAdvancedInventoryUIInterface.
	 * @param SearchParams The parameters used to search for the item.
	 * 
	 * @return The found inventory item, or an invalid item if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find Item (From UI Component)")
	static FInventoryItem FindItem(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FInventoryItemSearchParams& SearchParams);


	/**
	 * Calculates the spawn location for an actions list based on the provided widget.
	 *
	 * @param ParentWidget The parent widget used for determining the spawn location. Must be a valid widget.
	 * 
	 * @return A FVector2D representing the calculated screen position where the actions list should spawn. Returns FVector2D::ZeroVector if the ParentWidget or its world is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static FVector2D GetActionsListSpawnLocation(UWidget* ParentWidget);

#pragma endregion
	
	// --- Theme  ------------------------------
#pragma region Theme

	/**
	 * Applies the specified theme to the given user widget.
	 * This function ensures that the provided widget is valid and executes the ApplyTheme logic via the Mountea Inventory Generic Widget Interface.
	 *
	 * @param Target The user widget to which the theme will be applied. Must be a valid object.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Theme", meta=(CustomTag="MounteaK2Setter"))
	static void ApplyTheme(UWidget* Target);

	/**
	 * Retrieves the theme configuration for the Mountea Advanced Inventory system.
	 *
	 * This method fetches the theme configuration by accessing the global settings
	 * for the Mountea Advanced Inventory system, resolving any configured BaseTheme.
	 *
	 * @return Pointer to UMounteaAdvancedInventoryThemeConfig if the configuration
	 *         is valid and successfully resolved. Returns nullptr if the configuration
	 *         is invalid or unavailable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Theme", meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventoryThemeConfig* GetThemeConfig();
	
	/**
	 * Applies a theme-based style transformation to a button style, allowing customization of corners.
	 *
	 * @param BaseBrush The base button style to which the transformations will be applied.
	 * @param Level The theme level that determines the specific stylization applied.
	 * @param bApplyCorner1 If true, applies the style to the top-left corner.
	 * @param bApplyCorner2 If true, applies the style to the top-right corner.
	 * @param bApplyCorner3 If true, applies the style to the bottom-left corner.
	 * @param bApplyCorner4 If true, applies the style to the bottom-right corner.
	 * @return A new button style object with the modified properties based on the provided theme and corner options.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"), meta=(AdvancedDisplay="2"))
	static FButtonStyle ApplyButtonStyle(
		const FButtonStyle& BaseBrush,
		EMounteaThemeLevel Level,
		const bool bApplyCorner1 = true,
		const bool bApplyCorner2 = true,
		const bool bApplyCorner3 = true,
		const bool bApplyCorner4 = true
	);

	/**
	 * Creates a customized FButtonStyle based on the given base button style and visual theme level.
	 *
	 * @param BaseBrush The base button style to use as a reference for creating the new style.
	 * @param Level The theme level to determine the visual adjustments for the button style.
	 * @return A new FButtonStyle object with customized normal, hovered, pressed, and disabled states, along with optional outline, padding, and sound settings.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"))
	static FButtonStyle MakeButtonStyle(
		const FButtonStyle& BaseBrush,
		EMounteaThemeLevel Level
	);

	/**
	 * Generates a customized FScrollBarStyle based on an existing source style, theme level, and state.
	 *
	 * @param SourceStyle The base scrollbar style from which the new style will be derived.
	 * @param Level Represents the theme level to apply specific visual adjustments.
	 * @param State Specifies the state of the element (e.g., Normal, Hovered, Active, or Disabled)
	 *              to determine which part of the scrollbar style to modify.
	 * @return A modified FScrollBarStyle instance reflecting the provided theme level and state adjustments.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"))
	static FScrollBarStyle MakeScrollBarStyle(
		const FScrollBarStyle& SourceStyle,
		const EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		const EMounteaThemeState State = EMounteaThemeState::Normal
	);

	/**
	 * Modifies the given scroll bar style based on the specified Mountea theme level.
	 *
	 * @param SourceStyle The original FScrollBarStyle to be updated.
	 * @param Level The theme level used for applying specific styling adjustments.
	 * @return A modified copy of the original FScrollBarStyle with updated properties according to the chosen theme level.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"))
	static FScrollBarStyle ApplyScrollBarStyle(
		const FScrollBarStyle& SourceStyle,
		const EMounteaThemeLevel Level = EMounteaThemeLevel::Primary
	);
	
	/**
	 * Constructs a new FSlateBrush with adjusted properties based on the given theme parameters.
	 *
	 * @param SourceBrush The original FSlateBrush to be used as a base for the new brush.
	 * @param Level Specifies the thematic level (Primary, Secondary, Tertiary) to be applied to the brush.
	 * @param State Specifies the current state (Normal, Hovered, Active, Disabled) that affects the coloring of the brush.
	 * @param Type Specifies the type of the theme (Text, Background, Default) which determines how the brush is modified.
	 * @return A new FSlateBrush with updated values based on the provided theming parameters.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"))
	static FSlateBrush MakeSlateBrush(
		const FSlateBrush& SourceBrush,
		EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		EMounteaThemeState State = EMounteaThemeState::Normal,
		EMounteaThemeType Type = EMounteaThemeType::Default
	);

	/**
	 * Applies a theme to a specified FSlateBrush based on the given parameters.
	 * Modifies the appearance of the brush based on the theme's level, state, and type.
	 *
	 * @param SourceBrush The original FSlateBrush to which the theme settings will be applied.
	 * @param Level The theme level to apply (e.g., primary, secondary).
	 * @param State The state of the theme (e.g., active, inactive).
	 * @param Type The type of the theme (e.g., solid, gradient).
	 * @param bApplyCorner1 Whether or not to apply changes to Corner 1 of the brush.
	 * @param bApplyCorner2 Whether or not to apply changes to Corner 2 of the brush.
	 * @param bApplyCorner3 Whether or not to apply changes to Corner 3 of the brush.
	 * @param bApplyCorner4 Whether or not to apply changes to Corner 4 of the brush.
	 * @return A new FSlateBrush with the applied theme settings and optional corner modifications.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"), meta=(AdvancedDisplay="4"))
	static FSlateBrush ApplySlateBrush(
		const FSlateBrush& SourceBrush,
		EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		EMounteaThemeState State = EMounteaThemeState::Normal,
		EMounteaThemeType Type = EMounteaThemeType::Default,
		const bool bApplyCorner1 = true,
		const bool bApplyCorner2 = true,
		const bool bApplyCorner3 = true,
		const bool bApplyCorner4 = true
	);
	
	/**
	 * Creates a modified FSlateBrushOutlineSettings instance by applying theme-based colors and corner radii.
	 *
	 * @param SourceOutline The original outline settings to be modified.
	 * @param Level The theme level used to determine the outline's color.
	 * @param State The theme state used to further refine the outline's color based on the current state.
	 * @param bApplyCorner1 Whether to apply the configured border radius to the first corner.
	 * @param bApplyCorner2 Whether to apply the configured border radius to the second corner.
	 * @param bApplyCorner3 Whether to apply the configured border radius to the third corner.
	 * @param bApplyCorner4 Whether to apply the configured border radius to the fourth corner.
	 * @return A new FSlateBrushOutlineSettings instance with modified colors and corner radii.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"), meta=(AdvancedDisplay="3"))
	static FSlateBrushOutlineSettings MakeSlateBrushOutline(const FSlateBrushOutlineSettings& SourceOutline,
		EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		EMounteaThemeState State = EMounteaThemeState::Normal,
		const bool bApplyCorner1 = true,
		const bool bApplyCorner2 = true,
		const bool bApplyCorner3 = true,
		const bool bApplyCorner4 = true
	);

	/**
	 *  Applies an outline to the input FSlateBrush based on the specified theme level and state.
	 *
	 *  This method modifies the outline settings of the given SourceBrush to reflect the applied
	 *  theme parameters and corner configurations.
	 *
	 *  @param SourceBrush The FSlateBrush to which the outline is to be applied.
	 *  @param Level The theme level that determines the outline style.
	 *  @param State The theme state used for additional outline customization.
	 *  @param bApplyCorner1 Determines whether the outline is applied to the first corner.
	 *  @param bApplyCorner2 Determines whether the outline is applied to the second corner.
	 *  @param bApplyCorner3 Determines whether the outline is applied to the third corner.
	 *  @param bApplyCorner4 Determines whether the outline is applied to the fourth corner.
	 *  @return A new FSlateBrush instance with the updated outline settings.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Styling", meta=(CustomTag="MounteaK2Getter"), meta=(AdvancedDisplay="3"))
	static FSlateBrush ApplySlateBrushOutline(const FSlateBrush& SourceBrush,
		EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		EMounteaThemeState State = EMounteaThemeState::Normal,
		const bool bApplyCorner1 = true,
		const bool bApplyCorner2 = true,
		const bool bApplyCorner3 = true,
		const bool bApplyCorner4 = true
	);

	/**
	 * Applies theme styling to a font info object.
	 *
	 * @param SourceFont The original FSlateFontInfo to be themed.
	 * @param Level The theme level to apply.
	 * @param State The theme state to apply.
	 * @return A new FSlateFontInfo with applied theme styling.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static FSlateFontInfo ApplySlateFontInfo(
		const FSlateFontInfo& SourceFont,
		const EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		const EMounteaThemeState State = EMounteaThemeState::Normal
	);

	/**
	 * Applies theme styling to a text block style object.
	 *
	 * @param SourceStyle The original FTextBlockStyle to be themed.
	 * @param Level The theme level to apply.
	 * @param State The theme state to apply.
	 * @return A new FTextBlockStyle with applied theme colors and styling.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Getter"))
	static FTextBlockStyle ApplyTextBlockStyle(
		const FTextBlockStyle& SourceStyle,
		const EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		const EMounteaThemeState State = EMounteaThemeState::Normal
	);

	/**
	 * Applies theme styling directly to a UTextBlock widget.
	 *
	 * @param TextBlock The text block widget to apply theming to.
	 * @param Level The theme level to apply.
	 * @param State The theme state to apply.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI", meta=(CustomTag="MounteaK2Setter"))
	static void ApplyTextBlockTheme(
		UTextBlock* TextBlock,
		const EMounteaThemeLevel Level = EMounteaThemeLevel::Primary,
		const EMounteaThemeState State = EMounteaThemeState::Normal
	);

#pragma endregion

	// --- Generic Widget  ------------------------------
#pragma region GenericWidget

	/**
	 * Refreshes the provided UserWidget if it implements the MounteaInventoryGenericWidgetInterface.
	 *
	 * This utility function checks if the passed UUserWidget instance is valid and whether
	 * it implements the UMounteaInventoryGenericWidgetInterface. If both conditions are
	 * satisfied, it triggers the execution of the RefreshWidget function on the Target widget.
	 *
	 * @param Target The UUserWidget to be refreshed. Must be valid and implement UMounteaInventoryGenericWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", meta=(CustomTag="MounteaK2Setter"))
	static void RefreshWidget(UWidget* Target);
	
#pragma endregion

	// --- Base Widget  ------------------------------
#pragma region BaseWidget

	/**
	 * Sets the reference to the owning inventory UI interface.
	 * This function allows the widget to be associated with a specific inventory UI.
	 *
	 * @param Target The UI interface to create the inventory UI for, must implement `IMounteaAdvancedBaseInventoryWidgetInterface`
	 * @param NewOwningInventoryUI A TScriptInterface reference to the inventory UI interface that owns this widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Base", meta=(CustomTag="MounteaK2Setter"))
	static void SetOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI);
	
#pragma endregion
	
	// --- Main UI  ------------------------------
#pragma region MainUI

	/**
	 * Creates and initializes the inventory UI widgets.
	 * @param Target The UI interface to create the inventory UI for
	 * @return True if UI was successfully created and initialized, otherwise false
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool CreateMainUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);
	
	/**
	 * Retrieves the visibility state of the main UI.
	 *
	 * This function determines the current visibility status of the main UI
	 * and returns an appropriate ESlateVisibility value.
	 *
	 * @param Target MounteaAdvancedInventoryUI Object
	 * @return The visibility state of the main UI.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Getter"))
	static ESlateVisibility GetMainUIVisibility(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the visibility of the main UI.
	 *
	 * @param Target The interface of the advanced inventory UI where the visibility will be set.
	 * @param Visibility The desired slate visibility to apply to the main UI.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Setter"))
	static void SetMainUIVisibility(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const ESlateVisibility Visibility);

	/**
	 * Initializes the main UI widget for the Mountea inventory system by linking it with a parent widget.
	 *
	 * @param Target The target widget that implements the IMounteaInventorySystemBaseWidgetInterface. This widget will have its main UI initialized.
	 * @param Parent The parent widget that implements the IMounteaAdvancedInventoryUIInterface. This parent is used to provide context or linking to the target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Setter"))
	static void InitializeMainUIWidget(const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Parent);

	/**
	 * Returns the inventory UI.
	 * @param Target The UI interface to get the inventory UI from
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Getter"))
	static UUserWidget* GetMainUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Removes the inventory UI from the viewport and cleans up resources.
	 * @param Target The UI interface to remove the inventory UI from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Main", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveMainUIWrapper(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);
	
#pragma endregion
	
	// --- Notification  ------------------------------
#pragma region Notification
	
	/**
	 * Retrieves the widget that contains and manages inventory notifications.
	 * @param Target The UI interface to get the notification container from
	 * @return A pointer to the user widget that serves as the notification container
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", meta=(CustomTag="MounteaK2Getter"))
	static UUserWidget* GetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the widget that will contain and manage inventory notifications.
	 * @param Target The UI interface to set the notification container for
	 * @param NewNotificationContainer The user widget to use as the notification container
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", meta=(CustomTag="MounteaK2Setter"))
	static void SetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, UUserWidget* NewNotificationContainer);

	/**
	 * Creates and displays a new inventory notification based on the provided data.
	 * @param Target The UI interface to create the notification in
	 * @param NotificationData The data structure containing all notification parameters (text, duration, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", meta=(CustomTag="MounteaK2Setter"))
	static void CreateInventoryNotification(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FInventoryNotificationData& NotificationData);

	/**
	 * Removes all currently active inventory notifications from the UI.
	 * @param Target The UI interface to remove notifications from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveInventoryNotifications(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

#pragma endregion
	
	// --- InventoryUI  ------------------------------
#pragma region InventoryUI

	/**
	 * Removes the inventory widget wrapper for the specified target.
	 *
	 * @param Target The target interface implementing IMounteaInventorySystemBaseWidgetInterface for which the inventory widget wrapper should be removed.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveInventoryWidgetWrapper(const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target);

	/**
	 * Sets the source inventory for a given target widget interface.
	 *
	 * @param Target The target widget interface that will have its source inventory set.
	 * @param ParentInventory The parent inventory interface to associate with the target widget.
	 * @return Returns true if the source inventory was successfully set; otherwise, returns false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool SetSourceInventory(const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory);

	/**
	 * Triggers the 'CategorySelected' functionality on a given target, indicating that a specific inventory category has been selected.
	 *
	 * @param Target The interface implementing the IMounteaAdvancedInventoryUIInterface, representing the inventory UI where the category selection operation should occur.
	 * @param SelectedCategoryId The identifier of the selected category that needs to be processed by the target.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void CategorySelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FString& SelectedCategoryId);

	/**
	 * Retrieves the selected category ID from the specified inventory UI target.
	 *
	 * @param Target The inventory UI interface target from which to retrieve the selected category ID.
	 * @return The selected category ID as a string. Returns "none" if the target object is invalid or the operation fails.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static FString GetSelectedCategoryId(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Triggers the ItemSelected action on the specified inventory target.
	 *
	 * @param Target A TScriptInterface referencing the target implementing the IMounteaAdvancedInventoryUIInterface.
	 *               Represents the inventory UI on which the action will be executed.
	 * @param ItemGuid A FGuid that uniquely identifies the item being selected within the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void ItemSelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FGuid& ItemGuid);

	/**
	 * Retrieves the GUID of the currently selected item from the provided inventory UI interface.
	 *
	 * @param Target A TScriptInterface reference to the inventory UI interface from which the selected item's GUID is retrieved.
	 * @return FGuid of the selected item if the interface is valid; otherwise, returns an empty FGuid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetSelectedItemGuid(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

#pragma endregion

	// --- Inventory
#pragma region Inventory
	/**
	 * Retrieves the parent inventory associated with this UI.
	 * @param Target The UI interface to get the parent inventory from
	 * @return The parent inventory interface that manages the items and slots
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the parent inventory for this UI.
	 * @param Target The UI interface to set the parent inventory on
	 * @param NewParentInventory The inventory interface to associate with this UI
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);

	/**
	 * Assigns an inventory UI owner to the given widget.
	 *
	 * @param Target The widget to which the owning inventory UI will be assigned.
	 * @param OwningInventoryUI The owning inventory UI interface implementation to be set on the target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	
#pragma endregion

	// --- Categories Wrapper Widget ------------------------------
#pragma region CategoriesWrapper

	/**
	 * Associates the specified widget with the provided inventory UI interface.
	 *
	 * @param Target The widget to associate with the inventory UI.
	 * @param OwningInventoryUI The inventory UI interface to associate with the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoriesWrapperOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * Sets the active category ID for the specified target widget.
	 * This method ensures the target widget implements the required interface before calling the appropriate function to set the active category ID.
	 *
	 * @param Target The widget target that will have its active category ID set. Must implement the UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface interface.
	 * @param ActiveCategoryId The category ID to set as active.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveCategoryId(UWidget* Target, const FString& ActiveCategoryId);

	/**
	 * Retrieves the active category ID from a specified widget.
	 *
	 * If the provided widget implements the `UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface`
	 * interface, this method will invoke the `GetActiveCategoryId` function on the target widget and return
	 * the result. If the widget is invalid or does not implement the interface, a default value is returned.
	 *
	 * @param Target The widget from which the active category ID is to be retrieved. Must implement the `UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface` interface to return specific data.
	 * @return The active category ID as a string if the widget is valid and conforms to the interface, or "none" as a default fallback.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Getter"))
	static FString GetActiveCategoryId(UWidget* Target);
	
#pragma endregion
	
	// --- Category Widget ------------------------------
#pragma region Category

	/**
	 * Sets the inventory category key for a widget.
	 *
	 * This function assigns a category identifier to the given widget, provided that
	 * the widget implements the required interface for advanced inventory category management.
	 * If the widget is valid and implements the required interface, the category key is updated.
	 *
	 * @param Target The target widget to which the category key will be assigned. Must be a valid widget
	 *               that implements the UMounteaAdvancedInventoryCategoryWidgetInterface.
	 * @param CategoryId The identifier of the inventory category to be assigned to the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryCategoryKey(UWidget* Target, const FString& CategoryId);

	/**
	 * Retrieves the inventory category key associated with a given UWidget.
	 *
	 * @param Target The UWidget for which the inventory category key is to be retrieved. Must implement the
	 *               UMounteaAdvancedInventoryCategoryWidgetInterface interface.
	 * @return The inventory category key as an FString if the Target is valid and implements the required interface.
	 *         Returns "none" if the Target is invalid or does not implement the interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryCategoryKey(UWidget* Target);

	/**
	 * Associates a UWidget with its owning inventory UI interface.
	 *
	 * @param Target The widget to associate with the owning inventory UI.
	 * @param OwningInventoryUI The interface of the inventory UI that owns the specified widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoryOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * Sets the active state of a target widget implementing the MounteaAdvancedInventoryCategoryWidgetInterface.
	 *
	 * @param Target     The widget to update the active state for. It must be valid and implement the required interface.
	 * @param bIsActive  A boolean indicating whether the widget should be set as active (true) or inactive (false).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveState(UWidget* Target, const bool bIsActive);
	
#pragma endregion	
	
	// --- Item Widget ------------------------------
#pragma region Item

	/**
	 * Retrieves the currently active item widget in the inventory UI.
	 *
	 * This function returns a pointer to the active item widget currently being displayed or interacted with.
	 *
	 * @param Target 
	 * @return A pointer to the UUserWidget representing the active item widget, or nullptr if no widget is active.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Getter"))
	static UUserWidget* GetActiveItemWidget(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);

	/**
	 * Sets the active item widget in the UI.
	 *
	 * @param Target 
	 * @param NewActiveItemWidget The new widget to be set as the active item widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveItemWidget(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, UUserWidget* NewActiveItemWidget);

	/**
	 * Sets the inventory item ID for the specified widget.
	 *
	 * This method assigns a unique identifier to the target widget if it implements
	 * the MounteaAdvancedInventoryItemWidgetInterface interface. The item ID
	 * represented by the `ItemGuid` parameter is used to link the widget to a specific inventory item.
	 *
	 * @param Target A pointer to the widget that will have its inventory item ID set. This must be a valid widget instance.
	 * @param ItemGuid The unique identifier of the inventory item to associate with the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryItemId(UWidget* Target, const FGuid& ItemGuid);

	/**
	 * Retrieves the Inventory Item ID associated with the given widget.
	 * This method checks if the provided widget implements the MounteaAdvancedInventoryItemWidgetInterface and,
	 * if so, retrieves the Inventory Item ID via the interface's Execute_GetInventoryItemId method.
	 * If the widget is invalid or does not implement the interface, an empty FGuid is returned.
	 *
	 * @param Target The widget from which the Inventory Item ID is to be retrieved.
	 * Must implement the MounteaAdvancedInventoryItemWidgetInterface.
	 * @return The Inventory Item ID as an FGuid if the widget is valid and implements the required interface.
	 * Returns an empty FGuid otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetInventoryItemId(UWidget* Target);

	/**
	 * Sets the owning inventory UI interface for the specified widget.
	 *
	 * @param Target The UWidget that will have its owning inventory UI set.
	 * @param OwningInventoryUI The interface that represents the owning inventory UI to be assigned to the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * Refreshes the provided widget to reflect item changes.
	 *
	 * @param Target The widget to refresh. Must implement the IMounteaAdvancedInventoryItemWidgetInterface.
	 * @param Quantity The quantity of the item to display on the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item - Refresh Item Widget")
	static void Item_RefreshWidget(UWidget* Target, const int32 Quantity);

	/**
	 * Sets the parent slot for the specified widget.
	 * This method is used to associate a target widget with a parent UI widget
	 * which represents the parent slot in the inventory system.
	 *
	 * @param Target The widget to set the parent slot for. Must implement the MounteaAdvancedInventoryItemWidgetInterface.
	 * @param ParentSlot The user widget that will be assigned as the parent slot for the target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item - Set Parent Slot")
	static void Item_SetParentSlot(UWidget* Target, UUserWidget* ParentSlot);

	/**
	 * Highlights or un-highlights an inventory item within the UI by interacting with the provided widget.
	 *
	 * @param Target The target widget that implements the UMounteaAdvancedInventoryItemWidgetInterface.
	 * @param bIsSelected A boolean indicating whether to highlight (true) or un-highlight (false) the item.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item - Highlight Item")
	static void Item_HighlightItem(UWidget* Target, const bool bIsSelected = false);
	
#pragma endregion

	// --- Item Action ------------------------------
#pragma region ItemActions

	/**
	 * Initializes the item action widget with the parent UI and item action data.
	 *
	 * @param Target The target widget. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 * @param ParentUI The parent UI interface that owns this item action widget.
	 * @param ItemActionClass The item action class associated with this widget.
	 * @param ParentWidget Inventory Item widget which owns this item action widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Setter"))
	static void ItemAction_InitializeItemAction(UUserWidget* Target,
		const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentUI,
		const TSoftClassPtr<UObject>& ItemActionClass,
		UWidget* ParentWidget);

	/**
	 * Checks if the item action associated with the specified target widget is enabled.
	 * This function checks if the target widget implements the MounteaAdvancedInventoryItemActionWidgetInterface
	 * and calls the IsActionEnabled method if available.
	 *
	 * @param Target The target widget from which to check the item action status. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 * 
	 * @return True if the item action is enabled; otherwise, false.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Is Action Enabled")
	static bool ItemAction_IsActionEnabled(UUserWidget* Target);

	/**
	 * Checks if the item action associated with the specified target widget is valid.
	 * This function checks if the target widget implements the MounteaAdvancedInventoryItemActionWidgetInterface
	 * and calls the IsActionValid method if available.
	 *
	 * @param Target The target widget from which to check the item action validity. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 * 
	 * @return True if the item action is valid; otherwise, false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Is Action Valid")
	static bool ItemAction_IsActionValid(UUserWidget* Target);

	/**
	 * Executes the item action associated with the specified target widget.
	 * This function checks if the target widget implements the MounteaAdvancedInventoryItemActionWidgetInterface
	 * and calls the ExecuteItemAction method if available.
	 *
	 * @param Target The target widget from which to execute the item action. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Execute Item Action")
	static void ItemAction_ExecuteItemAction(UUserWidget* Target);

	/**
	 * Retrieves the item action class associated with the specified target widget.
	 * This function checks if the target widget implements the MounteaAdvancedInventoryItemActionWidgetInterface
	 * and retrieves the item action class if available.
	 *
	 * @param Target The target widget from which to retrieve the item action class. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 * 
	 * @return A TSoftClassPtr to the UMounteaInventoryItemAction class if available; otherwise, returns nullptr.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Item Action")
	static TSoftClassPtr<UObject> ItemAction_GetItemAction(UUserWidget* Target);

	/**
	 * Retrieves the action data associated with a specific target widget.
	 * This function checks if the target widget is valid and implements the MounteaAdvancedInventoryItemActionWidgetInterface,
	 * then retrieves the action data from it.
	 *
	 * @param Target The target widget from which to retrieve the action data. Must be a valid widget implementing the interface.
	 * 
	 * @return The FMounteaItemActionData containing the action's display information, or an empty struct if the target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Item Action Data")
	static FMounteaItemActionData ItemAction_GetActionData(UWidget* Target);

#pragma endregion

	// --- Item Actions Container------------------------
#pragma region ItemActionsContainer

	/**
	 * Sets the parent item widget for the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ParentItemWidget The parent item widget to be set for the item actions container.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Set Parent Item Widget")
	static void ItemActionsContainer_SetParentItemWidget(UWidget* Target, UWidget* ParentItemWidget);

	/**
	 * Constructs the item actions container from a list of item action classes.
	 *
	 * @param Target The target widget. Must implement the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionsList An array of TSoftClassPtr<UObject> representing the item actions to be added to the container.
	 * 
	 * Each class must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Construct From Actions List")
	static void ItemActionsContainer_ConstructFromActionsList(UUserWidget* Target, const TArray<TSoftClassPtr<UObject>>& ItemActionsList);

	/**
	 * Initializes the item actions container widget with the parent UI.
	 *
	 * @param Target The target widget. Must implement the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionWidget The widget to be added to the container. Must implement IMounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Add Item Action To Container")
	static void ItemActionsContainer_AddItemActionToContainer(UUserWidget* Target, UUserWidget* ItemActionWidget);

	/**
	 * Removes the specified item action widget from the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionWidget The item action widget to be removed from the container.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Remove Item Action From Container")
	static void ItemActionsContainer_RemoveItemActionFromContainer(UUserWidget* Target, UUserWidget* ItemActionWidget);

	/**
	 * Clears all item action widgets from the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Clear Item Actions Container")
	static void ItemActionsContainer_ClearItemActionsContainer(UUserWidget* Target);

	/**
	 * Selects a specific item action widget within the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionWidget The item action widget to be selected.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Select Item Action")
	static void ItemActionsContainer_SelectItemAction(UUserWidget* Target, UUserWidget* ItemActionWidget);

	/**
	 * Retrieves all item action widgets currently present in the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @return An array of UUserWidget pointers representing the item actions in the container.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Item Actions In Container")
	static TArray<UUserWidget*> ItemActionsContainer_GetItemActionsInContainer(UUserWidget* Target);

	/**
	 * Selects an item action widget by its index within the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionIndex The index of the item action to be selected.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Get Item Actions In Container By Index")
	static void ItemActionsContainer_SelectItemActionByIndex(UUserWidget* Target, int32 ItemActionIndex);
	
#pragma endregion
	
	// --- Item Tooltip ------------------------------
#pragma region ItemTooltip

	/**
	 * Blueprint implementable event allowing customization of a tooltip to display information
	 * about a specific inventory item.
	 *
	 * This function is designed to be implemented in Blueprints to update the tooltip's
	 * content based on the provided Item ID. The implementation can be tailored to show
	 * relevant information about the item tied to the given ID.
	 *
	 * @param Target The target widget that implements the UMounteaAdvancedInventoryItemTooltipWidgetInterface.
	 * @param SourceSlot Slot that contains the item information to be displayed in the tooltip.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemTooltip",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Tooltip - Set Tooltip Item")
	static void ItemTooltip_SetTooltipItem(UWidget* Target, const FInventorySlot& SourceSlot);
	
#pragma endregion
	
	// --- Item Slot Widget ------------------------------
#pragma region ItemSlot

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Slot Data",
		meta=(NativeMakeFunc))
	static FInventorySlot MakeInventoryGridSlotData(const FInventorySlot& SlotData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Slot Data",
		meta=(NativeMakeFunc))
	static FInventorySlot MakeInventorySlotData(const FMounteaInventoryGridSlot& GridSlotData);

	/**
	 * Create new inventory slot widget with the specified parameters.
	 * @param UserWidget The user widget that will contain the inventory slot.
	 * @param ItemId The unique identifier of the item to be displayed in the slot.
	 * @param Quantity The quantity of the item to be displayed in the slot.
	 * @return A new FInventorySlot instance containing the item ID and quantity.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Slot Data",
		meta=(NativeMakeFunc))
	static FInventorySlot MakeInventorySlot(UUserWidget* UserWidget, const FGuid& ItemId, const int32 Quantity);

	/**
	 * Retrieves the tooltip text for a given item slot widget.
	 * @param Target The target widget from which to retrieve the tooltip text. Must implement the UMounteaAdvancedInventoryItemSlotWidgetInterface.
	 * @return The tooltip text for the item slot, or an empty string if the widget is invalid or does not implement the required interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Slot Tooltip (Item Slot)")
	static FString ItemSlot_GetSlotTooltip(UUserWidget* Target);

	/**
	 * Generates a tooltip for the specified item slot widget.
	 * @param Target The target widget for which the tooltip will be generated. Must implement the UMounteaAdvancedInventoryItemSlotWidgetInterface.
	 * @return A string containing the generated tooltip text for the item slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
			meta=(CustomTag="MounteaK2Getter"),
			DisplayName="Generate Slot Tooltip (Item Slot)")
	static FString ItemSlot_GenerateSlotTooltip(UWidget* Target);

	/**
	 * Sets the owning inventory UI for a given item slot widget.
	 *
	 * @param Target The target widget item slot to associate with the owning inventory UI.
	 * @param OwningInventoryUI The inventory UI interface to set as the owner of the target item slot.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * Adds an item to the specified UI widget slot using the provided Item ID.
	 *
	 * @param Target The target widget to which the item will be added. Expected to implement UMounteaAdvancedInventoryItemSlotWidgetInterface.
	 * @param ItemId The unique identifier of the item to add to the specified slot.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Slot - Add Item To Slot")
	static void ItemSlot_AddItemToSlot(UWidget* Target, const FGuid& ItemId);

	/**
	 * Removes an item from the specified item slot widget.
	 *
	 * This method checks if the provided widget is valid and implements the appropriate interface,
	 * then executes the action to remove the item from the slot based on the provided item ID.
	 *
	 * @param Target The target widget from which the item should be removed. Must implement UMounteaAdvancedInventoryItemSlotWidgetInterface.
	 * @param ItemId The unique identifier (FGuid) of the item to be removed.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Slot - Remove Item From Slot")
	static void ItemSlot_RemoveItemFromSlot(UWidget* Target, const FGuid& ItemId);
	
	/**
	 * Stores the grid slot data for inventory purposes.
	 * This is optional information not all Slots need have (non-grid
	 * based inventory layouts won't have any grid slot data).
	 *
	 * @param Target  
	 * @param SlotData The grid slot data to be stored.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"))
	static void StoreGridSlotData(UWidget* Target, const FMounteaInventoryGridSlot& SlotData);

	/**
	 * Retrieves the grid slot data associated with a given widget.
	 *
	 * This method attempts to fetch the FMounteaInventoryGridSlot data from the specified UWidget.
	 * If the target widget implements the UMounteaAdvancedInventoryItemSlotWidgetInterface, the method
	 * retrieves and returns the associated grid slot data. If the target widget is invalid or does not
	 * implement the required interface, a default FMounteaInventoryGridSlot is returned.
	 *
	 * @param Target The widget for which the grid slot data will be retrieved. It must implement the
	 * UMounteaAdvancedInventoryItemSlotWidgetInterface to provide grid slot data.
	 * @return FMounteaInventoryGridSlot The grid slot data associated with the given widget, or a default
	 * grid slot if the widget is invalid or incompatible.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Item Slot - Get Item Slot Data")
	static FMounteaInventoryGridSlot GetGridSlotData(UWidget* Target);

	/**
	 * Retrieves the item widget currently in the inventory slot.
	 *
	 * @param Target 
	 * @return The item widget in the slot, or nullptr if no widget is present.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Item Slot - Get Item Widget In Slot")
	static UUserWidget* ItemSlot_GetItemWidgetInSlot(UWidget* Target);

	/**
	 * Handles the logic for selecting an item in the corresponding inventory slot.
	 * 
	 * @param Target 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Slot - Select Item In Slot")
	static void ItemSlot_SelectItemInSlot(UWidget* Target);

	/**
	 * Sets the parent widget that contains the slot.
	 *
	 * @param Target 
	 * @param ParentSlotsWrapper The parent widget that contains the slots.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Slot - Set Parent Slots Wrapper")
	static void ItemSlot_SetParentSlotsWrapper(UWidget* Target, UUserWidget* ParentSlotsWrapper);
	
#pragma endregion

	// --- Item Slots Wrapper ------------------------------
#pragma region ItemSlotsWrapper

	/**
	 * Sets the owning inventory UI for the specified widget target.
	 *
	 * @param Target The target widget for which the owning inventory UI is being set.
	 * @param OwningInventoryUI The interface of the advanced inventory UI to be set as the owner.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotsWrapperOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * Adds an item to the specified Target widget, identified by the given ItemId.
	 *
	 * @param Target The UI widget that implements the MounteaAdvancedInventoryItemSlotsWrapperWidgetInterface.
	 *               It serves as the target to add the item.
	 * @param ItemId The unique identifier (GUID) of the item to be added to the Target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Slots Wrapper - Add Item")
	static void SlotsWrapper_AddItem(UWidget* Target, const FGuid& ItemId);

	/**
	 * Updates an inventory item slot in the provided User Widget by invoking the appropriate interface method.
	 *
	 * @param Target The User Widget instance where the inventory slots are managed.
	 *               This must implement the UMounteaAdvancedInventoryItemSlotsWrapperWidgetInterface.
	 * @param ItemId The unique identifier (GUID) of the inventory item to be updated.
	 * @param OptionalItemSlot An optional integer specifying the specific slot where the update should occur.
	 *                         If not provided, the default behavior is applied by the interface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Slots Wrapper - Update Item")
	static void SlotsWrapper_UpdateItem(UWidget* Target, const FGuid& ItemId, const int32 OptionalItemSlot = -1);

	/**
	 * Removes an item with the specified ItemId from the Slots Wrapper within the given Target widget.
	 * If Quantity is not provided, all Items are expected to be removed.
	 *
	 * @param Target The widget that implements the Slots Wrapper interface where the item will be removed.
	 * @param ItemId The unique identifier of the item to be removed from the Slots Wrapper.
	 * @param Quantity How many units of the item should be removed from Inventory UI.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Slots Wrapper - Remove Item")
	static void SlotsWrapper_RemoveItem(UWidget* Target, const FGuid& ItemId, const int32 Quantity = -1);

	/**
	 * Retrieves the currently selected item widget.
	 *
	 * @param Target The widget that implements the Slots Wrapper.
	 * @return Returns a pointer to the selected UUserWidget instance.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Slots Wrapper - Get Selected Item Widget")
	static UUserWidget* SlotsWrapper_GetSelectedItemWidget(UWidget* Target);

	/**
	 * Attempts to set Item Widget as Selected one.
	 *
	 * @param Target The widget that implements the Slots Wrapper.
	 * @param NewSelectedItemWidget The widget to set as the currently selected item widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Slots Wrapper - Set Selected Item Widget")
	static void SlotsWrapper_SetSelectedItemWidget(UWidget* Target, UUserWidget* NewSelectedItemWidget);
	
#pragma endregion

// --- Items Grid ------------------------------
#pragma region ItemsGrid

	/**
	 * Adds an item to the first available (empty) slot in the grid.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param ItemId The unique identifier of the item to add
	 * @return True if the item was successfully added; false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Add Item To Empty Slot")
	static bool ItemsGrid_AddItemToEmptySlot(UWidget* Target, const FGuid& ItemId);

	/**
	 * Adds an item to a specific slot in the grid.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param ItemId The unique identifier of the item to add
	 * @param SlotIndex The index of the slot to place the item into
	 * @return True if the item was successfully added; false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Add Item To Slot")
	static bool ItemsGrid_AddItemToSlot(UWidget* Target, const FGuid& ItemId, const int32 SlotIndex);

	/**
	 * Removes an item from the specified slot.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param SlotIndex The index of the slot from which to remove the item
	 * @return True if the item was successfully removed; false if the slot is empty or invalid
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Remove Item From Slot")
	static bool ItemsGrid_RemoveItemFromSlot(UWidget* Target, const int32 SlotIndex);

	/**
	 * Removes an item from the inventory grid.
	 *
	 * This method attempts to remove a specified quantity of an item, identified by its unique ID, from the inventory grid.
	 * If the Quantity parameter is set to -1, it removes all occurrences of the item.
	 *
	 * @param Target The items grid widget interface to operate on
	 * @param ItemId The unique identifier (GUID) of the item to be removed.
	 * @param Quantity The number of items to remove. Defaults to -1, which removes all instances of the item.
	 * @return Returns true if the item(s) were successfully removed; otherwise, returns false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Remove Item")
	static bool ItemsGrid_RemoveItem(UWidget* Target, const FGuid& ItemId, const int32 Quantity = -1);

	/**
	 * Gets the item ID from a specific slot.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param SlotIndex The index of the slot to query
	 * @return The unique identifier (FGuid) of the item in the slot. 
	 *         If the slot is empty, returns an invalid FGuid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Item In Slot")
	static FGuid ItemsGrid_GetItemInSlot(UWidget* Target, const int32 SlotIndex);

	/**
	 * Swaps the items between two specific slots.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param SlotIndex1 The index of the first slot
	 * @param SlotIndex2 The index of the second slot
	 * @return True if the swap was successful; false if either slot is invalid
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Swap Items Between Slots")
	static bool ItemsGrid_SwapItemsBetweenSlots(UWidget* Target, const int32 SlotIndex1, const int32 SlotIndex2);

	/**
	 * Clears all items from every slot in the grid.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * This is useful for resetting or clearing the inventory grid entirely
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Items Grid - Clear All Slots")
	static void ItemsGrid_ClearAllSlots(UWidget* Target);

	/**
	 * Gets the total number of slots in the grid.
	 * 
	 * @param Target The items grid widget interface to query
	 * @return The total number of slots available in the inventory grid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Total Slots")
	static int32 ItemsGrid_GetTotalSlots(UWidget* Target);

	/**
	 * Checks if a specific slot is empty.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param SlotIndex The index of the slot to check
	 * @return True if the slot is empty; false if it contains an item
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Is Slot Empty")
	static bool ItemsGrid_IsSlotEmpty(UWidget* Target, const int32 SlotIndex);

	/**
	 * Gets the slot index that contains the specified item.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param ItemId The unique identifier (FGuid) of the item to locate
	 * @return The index of the slot that contains the item, or -1 if the item is not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Slot Index By Item")
	static int32 ItemsGrid_GetSlotIndexByItem(UWidget* Target, const FGuid& ItemId);

	/**
	 * Retrieves the data of a specific inventory grid slot based on coordinates.
	 *
	 * This method returns information about a grid slot based on its coordinates.
	 * It is used to fetch details such as the position of the slot and the item it contains.
	 *
	 * @param Target The items grid widget interface to query
	 * @param SlotCoords The coordinates of the grid slot to retrieve data for.
	 * @return FMounteaInventoryGridSlot containing information about the specified slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Slot Index By Coords")
	static int32 ItemsGrid_GetGridSlotIndexByCoords(UWidget* Target, const FIntPoint& SlotCoords);

	/**
	 * Checks if a specific item exists anywhere in the grid.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param ItemId The unique identifier of the item to check for
	 * @return True if the item is found in the grid; false if it is not present
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Is Item In Grid")
	static bool ItemsGrid_IsItemInGrid(UWidget* Target, const FGuid& ItemId);

	/**
	 * Retrieves the data of a specific inventory grid slot.
	 *
	 * @param Target The items grid widget interface to query
	 * @param SlotIndex The index of the grid slot to retrieve data for
	 * @return FMounteaInventoryGridSlot containing information about the specified slot
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Grid Slot Data")
	static FMounteaInventoryGridSlot ItemsGrid_GetGridSlotData(UWidget* Target, const int32 SlotIndex);

	/**
	 * Retrieves the data of all inventory grid slots.
	 *
	 * @param Target The items grid widget interface to query
	 * @return A set of FMounteaInventoryGridSlot containing the data for all slots in the grid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Grid Slots Data")
	static TSet<FMounteaInventoryGridSlot> ItemsGrid_GetGridSlotsData(UWidget* Target);

	/**
	 * Retrieves the item widget in the specified slot index from a target widget.
	 *
	 * This function checks if the target widget implements the
	 * UMounteaAdvancedInventoryItemsGridWidgetInterface interface and, if valid,
	 * executes the function to get the item widget in the given slot.
	 *
	 * @param Target The target widget to retrieve the item widget from.
	 * @param SlotIndex The index of the slot to retrieve the item widget for.
	 * @return A pointer to the UUserWidget representing the item widget in the specified slot,
	 *         or nullptr if the target is invalid or does not implement the necessary interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Get Item Wifget From Grid Slot")
	static UUserWidget* ItemsGrid_GetItemWidgetInSlot(UWidget* Target, const int32 SlotIndex);

	/**
	 * Attempts to find an empty widget slot within the specified target widget.
	 *
	 * @param Target The target user widget implementing MounteaAdvancedInventoryItemsGridWidgetInterface.
	 * @return A pointer to the found empty widget slot, or nullptr if no empty slot exists or the target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Find Empty Widget Slot")
	static UUserWidget* ItemsGrid_FindEmptyWidgetSlot(UWidget* Target);

	/**
	 * Finds the index of the first empty slot within a grid managed by a specified user widget.
	 *
	 * @param Target The target user widget that implements the MounteaAdvancedInventoryItemsGridWidgetInterface.
	 * @param ItemId The unique identifier of the item to check for
	 * @return The index of the first empty slot if found, otherwise INDEX_NONE.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find Empty Slot Index")
	static int32 ItemsGrid_FindEmptySlotIndex(UWidget* Target, const FGuid& ItemId);

	/**
	 * Adds a new slot to the inventory grid widget.
	 *
	 * This function allows adding a slot with specified data to the grid, enabling
	 * customization and extension of the inventory UI.
	 *
	 * @param Target The target user widget that implements the MounteaAdvancedInventoryItemsGridWidgetInterface.
	 * @param SlotData The data representing the slot to be added to the inventory grid.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Items Grid - Add Grid Slot")
	static void ItemsGrid_AddSlot(UWidget* Target, const FMounteaInventoryGridSlot& SlotData);

	/**
	 * Helper function to find an empty grid slot index in a given inventory widget for a specific item.
	 *
	 * This function checks if the provided widget is valid, implements the required interface, verifies the item ID,
	 * and ensures the parent inventory object is valid and implements the appropriate inventory interfaces. It performs
	 * a recursive search based on inventory settings to locate an appropriate empty slot index.
	 *
	 * @param Target The inventory widget to search for available grid slots. Must implement the required interface.
	 * @param ItemId The unique identifier of the item to find a slot for. Must be a valid FGuid.
	 * @param ParentInventory The parent inventory object associated with the widget. Must implement the inventory interface.
	 * @return The index of an available grid slot if found, or INDEX_NONE if no suitable slot is available.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Items Grid - Find Empty Slot Index (Helper)")
	static int32 Helper_FindEmptyGridSlotIndex(const UWidget* Target, const FGuid& ItemId, UObject* ParentInventory);
	static int32 FindEmptyGridSlotRecursive(TScriptInterface<IMounteaAdvancedInventoryInterface>& InventoryInterface, const FInventoryItem& InventoryItem, const TArray<FMounteaInventoryGridSlot>& GridSlots, const bool bIsStackable, const bool bAlwaysStackItems);

	/**
	 * Updates the item located in a specific slot of the inventory grid.
	 *
	 * This method attempts to update the item in the specified slot index by using the provided item ID.
	 * If a slot index is not specified, the default slot index (0) will be used.
	 *
	 * @param Target The target user widget that implements the MounteaAdvancedInventoryItemsGridWidgetInterface.
	 * @param ItemId A globally unique identifier (GUID) for the item to be updated.
	 * @param SlotIndex The index of the slot in which the item is updated (default is 0).
	 * @return Returns true if the item was successfully updated, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Items Grid - Update Item")
	static bool  ItemsGrid_UpdateItemInSlot(UWidget* Target, const FGuid& ItemId, const int32 SlotIndex = 0);
	static bool Helper_ItemsGrid_UpdateItemInSlot(UUserWidget* GridWidget, 
		const FGuid& ItemId, 
		int32 SlotIndex,
		TScriptInterface<IMounteaAdvancedInventoryUIInterface> ParentUIComponent);
	
#pragma endregion

// --- Items Preview ------------------------------
#pragma region ItemsPreview

	/**
	 * Initializes the given Interactable Widget.
	 *
	 * @param Target A pointer to the UMounteaAdvancedInventoryInteractableObjectWidget that needs initialization.
	 * @return True if the Target widget is valid and initialized successfully, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="ItemPreview - Initialize Interactable Widget")
	static bool ItemPreview_InitializeInteractableWidget(UMounteaAdvancedInventoryInteractableObjectWidget* Target);

	/**
	 * Sets the preview mesh for the specified target widget.
	 *
	 * @param Target The target widget to set the preview mesh on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param StaticMesh The static mesh to be used as the preview mesh for the target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Preview Mesh")
	static void ItemPreview_SetPreviewMesh(UMounteaAdvancedInventoryInteractableObjectWidget* Target, UStaticMesh* StaticMesh);

	/**
	 * Sets the preview mesh for the specified target widget.
	 *
	 * @param Target The target widget to set the preview mesh on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param SkeletalMesh The skeletal mesh to be used as the preview mesh for the target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Preview Skeletal Mesh")
	static void ItemPreview_SetPreviewSkeletalMesh(UMounteaAdvancedInventoryInteractableObjectWidget* Target, USkeletalMesh* SkeletalMesh);

	/**
	 * Clears the preview of the given inventory interactable object widget.
	 *
	 * @param Target A pointer to the UMounteaAdvancedInventoryInteractableObjectWidget for which the preview will be cleared. Must be a valid widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Clear Preview")
	static void ItemPreview_ClearPreview(UMounteaAdvancedInventoryInteractableObjectWidget* Target);

	/**
	 * Resets the item preview for the specified interactable object widget.
	 *
	 * @param Target The target widget representing an advanced inventory interactable object.
	 * The camera for this widget will be reset to its default state if it is valid.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Reset Preview")
	static void ItemPreview_ResetPreview(UMounteaAdvancedInventoryInteractableObjectWidget* Target);

	/**
	 * Updates camera rotation based on mouse delta input for the specified target widget.
	 *
	 * @param Target The target widget to update camera rotation on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param MouseDelta The mouse movement delta values for rotation calculation.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Rotation (Mouse)")
	static void ItemPreview_UpdateCameraRotation(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2D& MouseDelta);

	/**
	 * Updates camera height based on mouse delta input for the specified target widget.
	 *
	 * @param Target The target widget to update camera height on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param MouseDelta The mouse movement delta values for height calculation.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Height (Mouse)")
	static void ItemPreview_UpdateCameraHeight(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2D& MouseDelta);

	/**
	 * Updates camera zoom based on mouse wheel input for the specified target widget.
	 *
	 * @param Target The target widget to update camera zoom on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param WheelDelta The mouse wheel delta value for zoom calculation.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Zoom (Mouse Wheel)")
	static void ItemPreview_UpdateCameraZoom(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float WheelDelta);

	/**
	 * Sets camera rotation to absolute values for the specified target widget.
	 *
	 * @param Target The target widget to set camera rotation on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param YawNormalized The normalized yaw value (0.0 to 1.0) representing the full rotation range.
	 * @param PitchNormalized The normalized pitch value (0.0 to 1.0) representing the full rotation range.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Camera Rotation Absolute (Slider)")
	static void ItemPreview_SetCameraRotationAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float YawNormalized, const float PitchNormalized);

	/**
	 * Sets camera height to absolute value for the specified target widget.
	 *
	 * @param Target The target widget to set camera height on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param HeightNormalized The normalized height value (0.0 to 1.0) representing the full height range.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Camera Height Absolute (Slider)")
	static void ItemPreview_SetCameraHeightAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float HeightNormalized);

	/**
	 * Sets camera zoom to absolute value for the specified target widget.
	 *
	 * @param Target The target widget to set camera zoom on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param ZoomNormalized The normalized zoom value (0.0 to 1.0) representing the full zoom range.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Camera Zoom Absolute (Slider)")
	static void ItemPreview_SetCameraZoomAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float ZoomNormalized);

	/**
	 * Updates camera rotation based on analog gamepad input for the specified target widget.
	 *
	 * @param Target The target widget to update camera rotation on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param AnalogInput The analog input values (-1.0 to 1.0) from gamepad stick for rotation calculation.
	 * @param DeltaTime The time elapsed since the last frame for frame-rate independent movement.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Rotation Analog (Gamepad)")
	static void ItemPreview_UpdateCameraRotationAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2D& AnalogInput, const float DeltaTime);

	/**
	 * Updates camera height based on analog gamepad input for the specified target widget.
	 *
	 * @param Target The target widget to update camera height on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param AnalogInput The analog input value (-1.0 to 1.0) from gamepad for height calculation.
	 * @param DeltaTime The time elapsed since the last frame for frame-rate independent movement.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Height Analog (Gamepad)")
	static void ItemPreview_UpdateCameraHeightAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float AnalogInput, const float DeltaTime);

	/**
	 * Updates camera zoom based on analog gamepad input for the specified target widget.
	 *
	 * @param Target The target widget to update camera zoom on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param AnalogInput The analog input value (-1.0 to 1.0) from gamepad for zoom calculation.
	 * @param DeltaTime The time elapsed since the last frame for frame-rate independent movement.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Zoom Analog (Gamepad)")
	static void ItemPreview_UpdateCameraZoomAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float AnalogInput, const float DeltaTime);
	
#pragma endregion
};
