// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryUIStatics.generated.h"

class UWidget;
class UMounteaAdvancedInventoryInterface;
class UMounteaAdvancedInventoryThemeConfig;
struct FMounteaInventoryGridSlot;
class IMounteaAdvancedInventoryCategoryWidgetInterface;
class IMounteaInventorySystemBaseWidgetInterface;

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
	/********************* TEMPLATES **********************/
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
	/*********************** INTERNAL *************************/
	/*************************************************************/
	
	static APlayerController* FindPlayerController(AActor* Actor, int SearchDepth);
	static void SetOwningInventoryUIInternal(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI);
	
public:

	/*************************************************************/
	/******************* BLUEPRINTABLE *********************/
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
	 * 
	 * @param SourceData 
	 * @param TargetData 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|GridSlots", meta=(CustomTag="MounteaK2Setter"))
	static void SetGridSlotData(UPARAM(ref) FMounteaInventoryGridSlot& SourceData, const FMounteaInventoryGridSlot& TargetData);

	/**
	 *
	 * @param SourceData 
	 * @param ItemId 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item", meta=(CustomTag="MounteaK2Setter"))
	static void StoreItem(UPARAM(ref) FMounteaInventoryGridSlot& SourceData, const FGuid& ItemId);

	/**
	 * 
	 * @param SourceData 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item", meta=(CustomTag="MounteaK2Setter"))
	static void ResetItem(UPARAM(ref) FMounteaInventoryGridSlot& SourceData);

	/**
	 * Converts the provided inventory grid slot data to a string representation.
	 *
	 * @param SourceData The source data representing the inventory grid slot to be converted.
	 * @return A string representing the inventory grid slot data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Theme", meta=(CustomTag="MounteaK2Getter"), DisplayName="ToString")
	static FString GridSlot_ToString(const FMounteaInventoryGridSlot& SourceData);

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
	 * 
	 * @param Target 
	 * @param Parent 
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
	 * 
	 * @param Target 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void RemoveInventoryWidgetWrapper(const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target);

	/**
	 * 
	 * @param Target 
	 * @param ParentInventory 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool SetSourceInventory(const TScriptInterface<IMounteaInventorySystemBaseWidgetInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& ParentInventory);
	
	/**
	 * 
	 * @param Target MounteaAdvancedInventoryUI Object
	 * @param SelectedCategoryId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void CategorySelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FString& SelectedCategoryId);

	/**
	 * 
	 * @param Target 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static FString GetSelectedCategoryId(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target);
	
	/**
	 * 
	 * @param Target 
	 * @param ItemGuid 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void ItemSelected(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& Target, const FGuid& ItemGuid);

	/**
	 * 
	 * @param Target 
	 * @return 
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
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	
#pragma endregion

	// --- Categories Wrapper Widget ------------------------------
#pragma region CategoriesWrapper

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoriesWrapperOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveCategoryId(UWidget* Target, const FString& ActiveCategoryId);

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Getter"))
	static FString GetActiveCategoryId(UWidget* Target);
	
#pragma endregion
	
	// --- Category Widget ------------------------------
#pragma region Category
	
	/**
	 * 
	 * @param Target 
	 * @param CategoryId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryCategoryKey(UWidget* Target, const FString& CategoryId);

	/**
	 * 
	 * @param Target  
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Getter"))
	static FString GetInventoryCategoryKey(UWidget* Target);

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoryOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 * @param Target 
	 * @param bIsActive 
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
	 * 
	 * @param Target 
	 * @param ItemGuid 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryItemId(UWidget* Target, const FGuid& ItemGuid);

	/**
	 * 
	 * @param Target 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetInventoryItemId(UWidget* Target);

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 * @param Target
	 * @param Quantity 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Refresh Item Widget")
	static void Item_RefreshWidget(UWidget* Target, const int32 Quantity);

	/**
	 * 
	 *
	 * @param Target 
	 * @param ParentSlot The parent slot to be assigned to the inventory item widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Set Parent Slot")
	static void Item_SetParentSlot(UWidget* Target, UUserWidget* ParentSlot);

	/**
	 * Highlights or un-highlights an inventory item within the UI by interacting with the provided widget.
	 *
	 * @param Target The target widget that implements the UMounteaAdvancedInventoryItemWidgetInterface.
	 * @param bIsSelected A boolean indicating whether to highlight (true) or un-highlight (false) the item.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Highlight Item")
	static void Item_HighlightItem(UWidget* Target, const bool bIsSelected = false);
	
#pragma endregion

	// --- Item Slot Widget ------------------------------
#pragma region ItemSlot

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 *
	 * @param Target 
	 * @param ItemId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Add Item To Slot")
	static void ItemSlot_AddItemToSlot(UWidget* Target, const FGuid& ItemId);

	/**
	 * 
	 *
	 * @param Target 
	 * @param ItemId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Remove Item From Slot")
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
	 * 
	 * @param Target 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Item Slot Data")
	static FMounteaInventoryGridSlot GetGridSlotData(UWidget* Target);

	/**
	 * Retrieves the item widget currently in the inventory slot.
	 *
	 * @param Target 
	 * @return The item widget in the slot, or nullptr if no widget is present.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Item Widget In Slot")
	static UUserWidget* ItemSlot_GetItemWidgetInSlot(UWidget* Target);

	/**
	 * Handles the logic for selecting an item in the corresponding inventory slot.
	 * 
	 * @param Target 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Select Item In Slot")
	static void ItemSlot_SelectItemInSlot(UWidget* Target);

	/**
	 * Sets the parent widget that contains the slot.
	 *
	 * @param Target 
	 * @param ParentSlotsWrapper The parent widget that contains the slots.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Set Parent Slots Wrapper")
	static void ItemSlot_SetParentSlotsWrapper(UWidget* Target, UUserWidget* ParentSlotsWrapper);
	
#pragma endregion

	// --- Item Slots Wrapper ------------------------------
#pragma region ItemSlotsWrapper

	/**
	 * 
	 * @param Target 
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotsWrapperOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);

	/**
	 * 
	 * @param Target 
	 * @param ItemId 
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Add Item")
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
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Update Item")
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
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Remove Item")
	static void SlotsWrapper_RemoveItem(UWidget* Target, const FGuid& ItemId, const int32 Quantity = -1);

	/**
	 * Retrieves the currently selected item widget.
	 *
	 * @param Target The widget that implements the Slots Wrapper.
	 * @return Returns a pointer to the selected UUserWidget instance.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Selected Item Widget")
	static UUserWidget* SlotsWrapper_GetSelectedItemWidget(UWidget* Target);

	/**
	 * Attempts to set Item Widget as Selected one.
	 *
	 * @param Target The widget that implements the Slots Wrapper.
	 * @param NewSelectedItemWidget The widget to set as the currently selected item widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"), DisplayName="Set Selected Item Widget")
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Add Item To Empty Slot")
	static bool ItemsGrid_AddItemToEmptySlot(UWidget* Target, const FGuid& ItemId);

	/**
	 * Adds an item to a specific slot in the grid.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param ItemId The unique identifier of the item to add
	 * @param SlotIndex The index of the slot to place the item into
	 * @return True if the item was successfully added; false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Add Item To Slot")
	static bool ItemsGrid_AddItemToSlot(UWidget* Target, const FGuid& ItemId, const int32 SlotIndex);

	/**
	 * Removes an item from the specified slot.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param SlotIndex The index of the slot from which to remove the item
	 * @return True if the item was successfully removed; false if the slot is empty or invalid
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Remove Item From Slot")
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Remove Item")
	static bool ItemsGrid_RemoveItem(UWidget* Target, const FGuid& ItemId, const int32 Quantity = -1);

	/**
	 * Gets the item ID from a specific slot.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param SlotIndex The index of the slot to query
	 * @return The unique identifier (FGuid) of the item in the slot. 
	 *         If the slot is empty, returns an invalid FGuid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Item In Slot")
	static FGuid ItemsGrid_GetItemInSlot(UWidget* Target, const int32 SlotIndex);

	/**
	 * Swaps the items between two specific slots.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * @param SlotIndex1 The index of the first slot
	 * @param SlotIndex2 The index of the second slot
	 * @return True if the swap was successful; false if either slot is invalid
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Swap Items Between Slots")
	static bool ItemsGrid_SwapItemsBetweenSlots(UWidget* Target, const int32 SlotIndex1, const int32 SlotIndex2);

	/**
	 * Clears all items from every slot in the grid.
	 * 
	 * @param Target The items grid widget interface to operate on
	 * This is useful for resetting or clearing the inventory grid entirely
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), DisplayName="Clear All Slots")
	static void ItemsGrid_ClearAllSlots(UWidget* Target);

	/**
	 * Gets the total number of slots in the grid.
	 * 
	 * @param Target The items grid widget interface to query
	 * @return The total number of slots available in the inventory grid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Total Slots")
	static int32 ItemsGrid_GetTotalSlots(UWidget* Target);

	/**
	 * Checks if a specific slot is empty.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param SlotIndex The index of the slot to check
	 * @return True if the slot is empty; false if it contains an item
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Is Slot Empty")
	static bool ItemsGrid_IsSlotEmpty(UWidget* Target, const int32 SlotIndex);

	/**
	 * Gets the slot index that contains the specified item.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param ItemId The unique identifier (FGuid) of the item to locate
	 * @return The index of the slot that contains the item, or -1 if the item is not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Slot Index By Item")
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Slot Index By Coords")
	static int32 ItemsGrid_GetGridSlotIndexByCoords(UWidget* Target, const FIntPoint& SlotCoords);

	/**
	 * Checks if a specific item exists anywhere in the grid.
	 * 
	 * @param Target The items grid widget interface to query
	 * @param ItemId The unique identifier of the item to check for
	 * @return True if the item is found in the grid; false if it is not present
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Is Item In Grid")
	static bool ItemsGrid_IsItemInGrid(UWidget* Target, const FGuid& ItemId);

	/**
	 * Retrieves the data of a specific inventory grid slot.
	 *
	 * @param Target The items grid widget interface to query
	 * @param SlotIndex The index of the grid slot to retrieve data for
	 * @return FMounteaInventoryGridSlot containing information about the specified slot
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Grid Slot Data")
	static FMounteaInventoryGridSlot ItemsGrid_GetGridSlotData(UWidget* Target, const int32 SlotIndex);

	/**
	 * Retrieves the data of all inventory grid slots.
	 *
	 * @param Target The items grid widget interface to query
	 * @return A set of FMounteaInventoryGridSlot containing the data for all slots in the grid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Grid Slots Data")
	static TSet<FMounteaInventoryGridSlot> ItemsGrid_GetGridSlotsData(UWidget* Target);

	/**
	 * 
	 * @param Target 
	 * @param SlotIndex 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Get Item Wifget From Grid Slot")
	static UUserWidget* ItemsGrid_GetItemWidgetInSlot(UWidget* Target, const int32 SlotIndex);

	/**
	 * Attempts to find an empty widget slot within the specified target widget.
	 *
	 * @param Target The target user widget implementing MounteaAdvancedInventoryItemsGridWidgetInterface.
	 * @return A pointer to the found empty widget slot, or nullptr if no empty slot exists or the target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Find Empty Widget Slot")
	static UUserWidget* ItemsGrid_FindEmptyWidgetSlot(UWidget* Target);

	/**
	 * Finds the index of the first empty slot within a grid managed by a specified user widget.
	 *
	 * @param Target The target user widget that implements the MounteaAdvancedInventoryItemsGridWidgetInterface.
	 * @param ItemId The unique identifier of the item to check for
	 * @return The index of the first empty slot if found, otherwise INDEX_NONE.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Find Empty Slot Index")
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), DisplayName="Add Grid Slot")
	static void ItemsGrid_AddSlot(UWidget* Target, const FMounteaInventoryGridSlot& SlotData);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Getter"), DisplayName="Find Empty Slot Index (Helper)")
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemsGrid", meta=(CustomTag="MounteaK2Setter"), meta=(ExpandBoolAsExecs="ReturnValue"), DisplayName="Update Item")
	static bool  ItemsGrid_UpdateItemInSlot(UWidget* Target, const FGuid& ItemId, const int32 SlotIndex = 0);
	static bool Helper_ItemsGrid_UpdateItemInSlot(UUserWidget* GridWidget, 
		const FGuid& ItemId, 
		int32 SlotIndex,
		TScriptInterface<IMounteaAdvancedInventoryUIInterface> ParentUIComponent);
	
#pragma endregion
};
