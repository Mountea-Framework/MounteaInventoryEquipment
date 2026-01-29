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
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIManagerInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventoryUIStatics.generated.h"

enum class EMounteaWidgetInputPhase : uint8;
enum class ECommonInputType : uint8;

struct FMounteaInventoryGridSlot;
struct FInventoryItemSearchParams;
struct FMounteaItemActionData;

class UMounteaAdvancedInventoryUISubsystem;

class UMounteaSelectableInventoryItemAction;
class UMounteaInventoryScrollBox;
class UVerticalBoxSlot;
class UWidget;
class UTextBlock;
class UMounteaAdvancedInventoryInterface;
class UMounteaAdvancedInventoryThemeConfig;
class UMounteaAdvancedInventoryUIConfig;
class UMounteaAdvancedInventoryInteractableObjectWidget;
class IMounteaAdvancedInventoryCategoryWidgetInterface;
class IMounteaAdvancedBaseInventoryWidgetInterface;
class IMounteaInventorySystemWrapperWidgetInterface;

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

public:	
	static APlayerController* FindPlayerController(AActor* Actor, int SearchDepth);
	static void SetOwningInventoryUIInternal(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& NewOwningInventoryUI);
	
	// --- UI Subsystem
#pragma region UISubsystem
	
	/**
	 * Returns Inventory UI Manager.
	 * 
	 * @param FromActor World Context to find local player from.
	 * @return Inventory UI Manager if possible, null otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(CustomTag="MounteaK2Getter"))
	static UObject* GetInventoryUIManager(AActor* FromActor);
	
	/**
	 * Returns local player subsystem which holds reusable references.
	 * 
	 * @param FromPlayerController World Context to find local player from.
	 * @return UI Subsystem if possible, null otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventoryUISubsystem* GetInventoryUISubsystem(APlayerController* FromPlayerController);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Helpers",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Inventory UI Subsystem")
	static UMounteaAdvancedInventoryUISubsystem* GetInventoryUISubsystem_Generic(UObject* Context);
	
#pragma endregion
	
	// --- UI Manager	
#pragma region UIManager
	
	/**
	 * Creates and initializes the Main Viewport Widget which contains all
	 * child widgets, like Inventory, Equipment, Health, Crafting and others.
	 * This is the main UI screen/container which should handle all UI related
	 * visual logic, like creating Inventory etc.
	 * 
	 * @param Target The UI Manager to create the Main Wrapper UI for
	 * @return True if UI was successfully created and initialized, otherwise false
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(Keywords="container,viewport,make,add"),
		DisplayName="Create Wrapper Widget")
	static bool CreateWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Initializes the Wrapper widget for the Mountea inventory system.
	 * UI Manager (IMounteaAdvancedInventoryUIManagerInterface) knows this wrapper/container
	 * and knows it processes all internal logic.
	 * 
	 * This is the main UI screen/container which should handle all UI related
	 * visual logic, like creating Inventory etc.
	 *
	 * @param Target The target widget that implements the IMounteaInventorySystemBaseWidgetInterface.
	 * @param Parent The parent UI Manager that implements the IMounteaAdvancedInventoryUIManagerInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="container,viewport,start"),
		DisplayName="Initialize Wrapper Widget")
	static void InitializeWrapperWidget(UObject* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Parent);

	/**
	 * Returns the Main Wrapper Widget.
	 * @param Target The UI Manager to get Wrapper widget from.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager", 
		meta=(CustomTag="MounteaK2Getter"),
		meta=(Keywords="container,viewport"),
		DisplayName="Get Wrapper Widget")
	static UUserWidget* GetWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Removes the Wrapper UI from the viewport and cleans up resources.
	 * This will destroy all child UI elements inside the Main Wrapper!
	 * 
	 * @param Target The UI Manager to remove the Wrapper UI from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="container,viewport,destroy"),
		DisplayName="Remove Wrapper Widget")
	static void RemoveWrapperWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Creates and initializes the Inventory Widget.
	 * Inventory Widget should be always child to Wrapper Widget!
	 * 
	 * @param Target The UI Manager to create the Inventory UI for
	 * @return True if UI was successfully created and initialized, otherwise false
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory", 
		meta=(CustomTag="MounteaK2Setter"), 
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(Keywords="make,add"),
		DisplayName="Create Inventory Widget")
	static bool CreateInventoryWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Returns the Inventory Widget.
	 * @param Target The UI Manager to get Inventory widget from.
	 * @return UI if UI exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Inventory Widget")
	static UUserWidget* GetInventoryWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Removes the Inventory UI from the viewport and cleans up resources.
	 * This will destroy all child UI elements inside the Inventory Widget!
	 * 
	 * @param Target The UI Manager to remove the Inventory UI from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="destroy"),
		DisplayName="Remove Wrapper Widget")
	static void RemoveInventoryWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Attempts to override the Inventory Widget.
	 * You can handle the logic inside your Manager, or if called from outside (like the Wrapper), you can call this function.
	 * 
	 * @param Target The UI Manager to set Inventory widget in.
	 * @param NewInventoryWidget Widget to be the new Inventory. Can be null to reset the stored value.
	 * @return False if attempting to set the same value.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Inventory", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Set Inventory Widget")
	static bool SetInventoryWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		UUserWidget* NewInventoryWidget);
	
	/**
	 * Finds an item in the inventory using the provided search parameters.
	 * This function retrieves the parent inventory from the target UI component
	 * and searches for the item based on the specified search parameters.
	 *
	 * @param Target The target UI component implementing MounteaAdvancedInventoryUIManagerInterface.
	 * @param SearchParams The parameters used to search for the item.
	 * 
	 * @return The found inventory item, or an invalid item if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Items",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Inventory UI Manager - Find Item")
	static FInventoryItem FindItem(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, const FInventoryItemSearchParams& SearchParams);
	
	/**
	 * Retrieves local custom items map.
	 * 
	 * @param Target     UI component implementing MounteaAdvancedInventoryUIManagerInterface.
	 * @return Custom Items Mpa if any specified.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Custom Items Map",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Inventory UI Manager - Get Custom Items")
	static TMap<FGameplayTag,FInventoryUICustomData> GetCustomItemsMap(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Adds (or replaces) a custom item entry in the target UI component's custom items map.
	 *
	 * @param Target     UI component implementing MounteaAdvancedInventoryUIManagerInterface.
	 * @param ItemTag    Gameplay tag representing the custom item key.
	 * @param ItemId     Guid value associated with the item.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Custom Items Map",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Add Custom Item")
	static void AddCustomItem(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, const FGameplayTag& ItemTag,
		const FGuid& ItemId);
	
	/**
	 * Appends custom item entries to the target UI component's custom items map.
	 * Existing keys will be overwritten.
	 *
	 * @param Target       UI component implementing MounteaAdvancedInventoryUIManagerInterface.
	 * @param OtherItems   Map of custom items to append.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Custom Items Map",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Append Custom Items")
	static void AppendCustomItems(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		const TMap<FGameplayTag, FInventoryUICustomData>& OtherItems);

	/**
	 * Clears all custom items from the target UI component's custom items map.
	 *
	 * @param Target     UI component implementing MounteaAdvancedInventoryUIManagerInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager|Custom Items Map",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Clear Custom Items")
	static void ClearCustomItems(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Removes a single custom item from the target UI component's custom items map.
	 *
	 * @param Target    UI component implementing MounteaAdvancedInventoryUIManagerInterface.
	 * @param ItemTag   Gameplay tag representing the custom item key to remove.
	 * @param ItemId	Guide of the item to be removed.
	 *
	 * @return True if an entry was removed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Custom Items Map",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),		
		DisplayName="Inventory UI Manager - Remove Custom Item")
	static bool RemoveCustomItem(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		const FGameplayTag& ItemTag, const FGuid& ItemId);
	
	/**
	 * Validates whether the provided Item guid is stored in a map for specified tag.
	 * Example:
	 * - Is Item in Favorites
	 * 
	 * @param Target UI manager implementing MounteaAdvancedInventoryUIManagerInterface.
	 * @param ItemTag Tag which defines the key, like "Favorite"
	 * @param ItemId Item guid to search for
	 * @return True if item is stored in such container, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Custom Items Map",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Inventory UI Manager - Is Item Stored In Custom Map")
	static bool IsItemStoredInCustomMap(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		const FGameplayTag& ItemTag, const FGuid& ItemId);
	
	/**
	 * This function allows UI Manager to listen to external systems which might want to process Widget Commands.
	 * Example might to a request to refresh item count when Item Action consumes it.
	 * 
	 * Chain of hierarchy is respected, so commands are given to root and then based on the selection passed down.
	 * 
	 * @param Target UI Manager to execute the logic on
	 * @param Command Command to Process
	 * @param OptionalPayload Optional Payload to pass with the command
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Commands",
		meta=(CustomTag="MounteaK2Setter,MounteaK2Command"),
		DisplayName="Inventory UI Manager - Execute Widget Command")
	static void ExecuteWidgetCommandFromManager(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		const FString& Command, UObject* OptionalPayload);
	
	/**
	 * Returns a snapshot of the currently queued Item Actions waiting to be processed.
	 *
	 * ItemActionsQueue order reflects processing order (FIFO), where the first element is the next action to execute.
	 *
	 * @param Target UI Manager to execute the logic on
	 * 
	 * @return Array of queued Item Actions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Item Actions",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Inventory UI Manager - Get Item Actions Queue")
	static TArray<UMounteaSelectableInventoryItemAction*> GetItemActionsQueue(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Enqueues a single Item Action to be executed by the UI Manager.
	 *
	 * The provided Payload is passed forward to the action execution and can represent any context data
	 * required for processing (such as the targeted item, owning inventory, UI widget, etc.).
	 *
	 * @param Target UI Manager to execute the logic on
	 * @param ItemAction Item Action to enqueue.
	 * @param Payload Optional context object passed to the queued action.
	 *
	 * @return True if the action was successfully enqueued, false otherwise (invalid action, queue disabled, etc.).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="add"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Inventory UI Manager - Enqueue Item Action")
	static bool EnqueueItemAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, 
		UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload);
	
	/**
	 * Removes all queued Selectable Item Actions from the UI Manager queue.
	 *
	 * This function clears the queue registry only. It does not attempt to complete or cancel individual actions,
	 * nor does it notify them. Any UI or async work owned by the actions remains their responsibility.
	 * 
	 * @param Target UI Manager to execute the logic on
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="clear"),
		DisplayName="Inventory UI Manager - Empty Item Actions Queue")
	static void EmptyItemActionsQueue_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Marks the specified queued Selectable Item Action as completed and removes it from the queue.
	 *
	 * This function is typically called by the action itself after its UI-gated flow has been approved and the action
	 * has executed its final logic (e.g. consume amount selected in a modal). The UI Manager uses this call to stop
	 * tracking the action in its pending registry.
	 *
	 * @param Target UI Manager to execute the logic on
	 * @param ItemAction Selectable Item Action to remove from the queue.
	 * @param Payload Optional payload received for completion of the Action. Usually contains "returned" values (eg. how much quantity was requested to consume).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Complete Item Action")
	static void CompleteQueuedAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		UMounteaSelectableInventoryItemAction* ItemAction, UObject* Payload);
	
	/**
	 * Cancels the specified queued Selectable Item Action and removes it from the queue.
	 *
	 * This function is typically called by the action itself when the UI-gated flow is declined/cancelled
	 * (e.g. modal closed, user pressed cancel). The UI Manager uses this call to stop tracking the action
	 * in its pending registry.
	 *
	 * @param Target UI Manager to execute the logic on
	 * @param ItemAction Selectable Item Action to remove from the queue.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Cancel Item Action")
	static void CancelQueuedAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		UMounteaSelectableInventoryItemAction* ItemAction);
	
	/**
	 * Removes the specified queued Selectable Item Action from the queue.
	 *
	 * This function is typically called by the action itself once the action is completed.
	 *
	 * @param Target UI Manager to execute the logic on
	 * @param ItemAction Selectable Item Action to remove from the queue.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager|Item Actions",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Remove Item Action")
	static void RemoveQueuedAction_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target,
		UMounteaSelectableInventoryItemAction* ItemAction);
	
#pragma endregion
	
	// --- Helpers	
#pragma region Helpers

	/**
	 * Checks whether the UI item represented by InventoryItemData is detached (not present) in the provided list of ValidItems.
	 *
	 * A detached UI item is considered "orphaned" from the valid items collection:
	 * - If ValidItems is empty, the item is treated as detached.
	 * - If InventoryItemData.ContainingItem is invalid, the item is treated as detached.
	 * - Otherwise, the function checks whether InventoryItemData.ContainingItem.Guid exists in ValidItems.
	 *
	 * @param ValidItems Collection of items that are considered valid/attached for UI purposes.
	 * @param InventoryItemData UI inventory item data containing the item reference (ContainingItem) to validate.
	 * @return True if the item is detached (not found in ValidItems or invalid input), false if the item exists in ValidItems.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|UI|Helpers", 
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Is Item UI Detached From Valid Items")
	static bool IsItemWidgetDetachedFromValidItems(const TArray<FInventoryItem>& ValidItems, const FInventoryItemData& InventoryItemData);
	
	/**
	 * Validates whether the specified hardware input type is currently active for the owning local player.
	 *
	 * This helper evaluates the active input method as reported by CommonUI (UCommonInputSubsystem) and compares it
	 * against the provided input type. Useful for switching UI glyphs, button hints, or input-specific behavior
	 * (keyboard/mouse vs gamepad vs touch) directly in Blueprint.
	 *
	 * @param ContextObject User widget used to resolve the owning local player and its CommonUI input subsystem.
	 * @param InputType Input type to validate against the currently active input method.
	 *
	 * @return True if the owning local player exists and its current input type matches InputType, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|UI|Helpers", 
		meta=(CustomTag="MounteaK2Validate"),
		meta=(DefaultToSelf="ContextObject"),
		DisplayName="Validate HW Input Type")
	static bool InInputTypeActive(const UUserWidget* ContextObject, const ECommonInputType& InputType);
	
	/**
	 * Centers a specific item in a list widget by applying render translation.
	 * Calculates the necessary offset to position the selected item in the vertical center of the viewport.
	 * 
	 * @param ListWidget The panel widget containing the list items (e.g., Vertical Box, Scroll Box content)
	 * @param SelectedIndex Zero-based index of the item to center in the viewport
	 * 
	 * @note Requires uniform item heights. Uses the first child's height for calculations.
	 * @note Cached geometry must be valid (call after first frame/tick).
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Helpers", 
		meta=(CustomTag="MounteaK2Setter"))
	static void CenterListItemAtIndex(class UPanelWidget* ListWidget, int32 SelectedIndex);
	
	/**
	 * Calculates the render translation needed to center a specific item in a list widget.
	 * Returns the translation vector without applying it, allowing for animated transitions.
	 * 
	 * @param ListWidget The panel widget containing the list items (e.g., Vertical Box, Scroll Box content)
	 * @param SelectedIndex Zero-based index of the item to center in the viewport
	 * @return The translation vector to apply via SetRenderTranslation
	 * 
	 * @note Requires uniform item heights. Uses the first child's height for calculations.
	 * @note Cached geometry must be valid (call after first frame/tick).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|UI|Helpers", 
		meta=(CustomTag="MounteaK2Getter"), DisplayName="Calculate Centered List Translation")
	static FVector2D CalculateCenteredListTranslation(UPanelWidget* ListWidget, int32 SelectedIndex);
	static FVector2D CalculateCenteredListTranslation(const TSharedPtr<SWidget>& ListWidget, const FGeometry& ListGeometry, const int32 SelectedIndex);
	
	/**
	 * Applies size to font.
	 * 
	 * @param Font Font to be updated.
	 * @param NewSize Size to apply.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Helpers", 
		meta=(CustomTag="MounteaK2Setter"))
	static FSlateFontInfo SetFontSize(const FSlateFontInfo& Font, const int32 NewSize);
	
	/**
	 * Applies typeface to font.
	 * 
	 * @param Font Font to be updated.
	 * @param NewTypeface Typeface to apply.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Helpers", 
		meta=(CustomTag="MounteaK2Setter"))
	static FSlateFontInfo SetFontTypeface(const FSlateFontInfo& Font, const FName& NewTypeface);
	
	/**
	 * Retrieves the advanced configuration settings for the Mountea Inventory System.
	 * Facilitates access to inventory-specific configurations like inventory types, rarities, and categories.
	 *
	 * @return - A pointer to the Mountea Advanced Inventory Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Inventory UI Config")
	static UMounteaAdvancedInventoryUIConfig* GetInventoryUISettingsConfig();

#pragma endregion
	
	// --- UI Input
#pragma region UIInput
	
	/**
	 * Tries to find UI action mapping Input Action based on the preferred tags.
	 *
	 * @param PreferredInputActions List of preferred Input Action Tags.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param OutMapping Returns found Mapping if any is found.
	 * 
	 * @return Returns true if a mapping was found. OutMapping contains the best match.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Input", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mapping (Item Action)")
	static bool FindUIActionMappingForItemAction(const FGameplayTagContainer& PreferredInputActions,
		const FGameplayTagContainer& WidgetStates, FMounteaWidgetInputActionMapping& OutMapping);
	
	/**
	 * Tries to resolve a UI action mapping from a Slate key event.
	 *
	 * @param KeyEvent Event to read data from. Keyboard event most of the time.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param Mappings Stored mappings to search in.
	 * @param OutMapping Returns found Mapping if any is found.
	 * 
	 * @return Returns true if a mapping was found. OutMapping contains the first match.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Input", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mapping (Key Event)")
	static bool FindUIActionMappingFromKeyEvent(const FKeyEvent& KeyEvent, const FGameplayTagContainer& WidgetStates,
		const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping);
	
	/**
	 * Tries to resolve a UI action mapping from a Slate analog input event.
	 *
	 * @param AnalogEvent Event to read data from. Gamepad axis / trigger / analog-like input.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param Mappings Stored mappings to search in.
	 * @param OutMapping Returns found Mapping if any is found.
	 * 
	 * @return Returns true if a mapping was found. OutMapping contains the first match.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Input", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mapping (Analog Event)")
	static bool FindUIActionMappingFromAnalogEvent(const FAnalogInputEvent& AnalogEvent, const FGameplayTagContainer& WidgetStates,
		const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping);
	
	/**
	 * Tries to resolve a UI action mapping from a Slate pointer event (mouse / touch / pen).
	 *
	 * For mouse button events, the "effecting button" is used.
	 * For mouse wheel events, this function resolves input as EKeys::MouseWheelAxis when WheelDelta != 0.
	 *
	 * @param MouseEvent Event to read data from. Mouse/touch pointer event most of the time.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param Mappings Stored mappings to search in.
	 * @param OutMapping Returns found Mapping if any is found.
	 * 
	 * @return Returns true if a mapping was found. OutMapping contains the first match.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Input", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mapping (Mouse Event)")
	static bool FindUIActionMappingFromMouseEvent(const FPointerEvent& MouseEvent, const FGameplayTagContainer& WidgetStates,
		const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping);
	
	/**
	 * Tries to resolve UI action mappings from a Slate key event.
	 *
	 * @param KeyEvent Event to read data from. Keyboard event most of the time.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param Mappings Stored mappings to search in.
	 * @param OutMappings Returns found mappings sorted by InputPriority (highest first).
	 * 
	 * @return Returns true if at least one mapping was found.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Input",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mappings (Key Event)")
	static bool FindUIActionMappingsFromKeyEvent(const FKeyEvent& KeyEvent, const FGameplayTagContainer& WidgetStates,
		const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings);
	
	/**
	 * Tries to resolve UI action mappings from a Slate analog input event.
	 *
	 * @param AnalogEvent Event to read data from. Gamepad axis / trigger / analog-like input.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param Mappings Stored mappings to search in.
	 * @param OutMappings Returns found mappings sorted by InputPriority (highest first).
	 * 
	 * @return Returns true if at least one mapping was found.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Input",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mappings (Analog Event)")
	static bool FindUIActionMappingsFromAnalogEvent(const FAnalogInputEvent& AnalogEvent, const FGameplayTagContainer& WidgetStates,
		const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings);
	
	/**
	 * Tries to resolve UI action mappings from a Slate pointer event (mouse / touch / pen).
	 *
	 * For mouse button events, the "effecting button" is used.
	 * For mouse wheel events, this function resolves input as EKeys::MouseWheelAxis when WheelDelta != 0.
	 *
	 * @param MouseEvent Event to read data from. Mouse/touch pointer event most of the time.
	 * @param WidgetStates Widget States from wrapper (or any in-focus widget) which behave as filters for KeyMappings.
	 * @param Mappings Stored mappings to search in.
	 * @param OutMappings Returns found mappings sorted by InputPriority (highest first).
	 * 
	 * @return Returns true if at least one mapping was found.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Input",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Find UI Input Mappings (Mouse Event)")
	static bool FindUIActionMappingsFromMouseEvent(const FPointerEvent& MouseEvent, const FGameplayTagContainer& WidgetStates,
		const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings);
	
	static bool FindUIActionMappingByKey(const FKey& PressedKey, const FModifierKeysState& Modifiers,
		const FGameplayTagContainer& WidgetStates, const TArray<FMounteaWidgetInputActionMapping>& Mappings, FMounteaWidgetInputActionMapping& OutMapping);
	static bool FindUIActionMappingsByKey(const FKey& PressedKey, const FModifierKeysState& Modifiers,
		const FGameplayTagContainer& WidgetStates, const TArray<FMounteaWidgetInputActionMapping>& Mappings, TArray<FMounteaWidgetInputActionMapping>& OutMappings);
	
#pragma endregion
	
	// --- MounteaInventoryScrollBox
#pragma region MounteaScrollBox

	/**
	 * Returns number of children currently stored in the ScrollBox.
	 *
	 * @param ScrollBox ScrollBox instance to read children from.
	 * @return Total number of child widgets contained in the ScrollBox. Returns 0 if ScrollBox is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Getter", Keywords="num,len"),
		DisplayName="Get Children Count")
	static int32 MounteaInventoryScrollBox_GetChildrenCount(const UMounteaInventoryScrollBox* ScrollBox);

	/**
	 * Returns active (selected/focused) child index used by the ScrollBox for navigation.
	 *
	 * The meaning of "active" depends on UMounteaInventoryScrollBox implementation (typically the item
	 * currently highlighted, focused, or treated as the navigation target).
	 *
	 * @param ScrollBox ScrollBox instance to read active index from.
	 * @return Current active index. Returns INDEX_NONE if ScrollBox is invalid or no active item is set.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Active Index")
	static int32 MounteaInventoryScrollBox_GetActiveIndex(const UMounteaInventoryScrollBox* ScrollBox);

	/**
	 * Sets active (selected/focused) child index used by the ScrollBox for navigation.
	 *
	 * The function is expected to clamp/validate the value internally. If the provided index is out of
	 * bounds, the implementation should either clamp to the nearest valid index or set INDEX_NONE.
	 *
	 * @param ScrollBox ScrollBox instance to modify.
	 * @param NewActiveIndex New active index to set (0..ChildrenCount-1), or INDEX_NONE to clear selection.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Set Active Index")
	static void MounteaInventoryScrollBox_SetActiveIndex(UMounteaInventoryScrollBox* ScrollBox, const int32 NewActiveIndex);

	/**
	 * Adds a child widget to the ScrollBox.
	 *
	 * @param ScrollBox ScrollBox instance to add the child to.
	 * @param Content Widget to be added as a child. No action is performed if Content is null or ScrollBox is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Add Child")
	static void MounteaInventoryScrollBox_AddChild(UMounteaInventoryScrollBox* ScrollBox, UWidget* Content);

	/**
	 * Removes all children from the ScrollBox and resets any internal navigation/selection state.
	 *
	 * @param ScrollBox ScrollBox instance to clear.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Reset Children")
	static void MounteaInventoryScrollBox_ResetChildren(UMounteaInventoryScrollBox* ScrollBox);

	/**
	 * Retrieves all child Widgets in the Scrollbox.
	 * @param ScrollBox Target Mountea Scrollbox.
	 * @return Array of UWidget objects, might be empty.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get All Children")
	static TArray<UWidget*> MounteaInventoryScrollBox_GetAllChildren(UMounteaInventoryScrollBox* ScrollBox);
	
	/**
	 * Removes specified child from the ScrollBox.
	 *
	 * @param Content Widget to be removed from Scrollbox.
	 * @param ScrollBox ScrollBox instance to clear.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Remove Child")
	static void MounteaInventoryScrollBox_RemoveChild(UMounteaInventoryScrollBox* ScrollBox, UWidget* Content);
	
	/**
	 * Removes specified child from the ScrollBox at given index.
	 *
	 * @param Index Index of the Widget to be removed from Scrollbox.
	 * @param ScrollBox ScrollBox instance to clear.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|UI|Scrollbox",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Remove Child At")
	static void MounteaInventoryScrollBox_RemoveChildAt(UMounteaInventoryScrollBox* ScrollBox, const int32 Index);

#pragma endregion
	
	// --- Wrapper	
#pragma region Wrapper
	
	static bool IsValidWrapperWidget(const UObject* Target);
	
	/**
	 * Sets the source inventory for a given Wrapper widget.
	 * This function should be used sparsely as it resets the Parent Inventory
	 * for all children elements of this Wrapper!
	 *
	 * @param Target The target widget interface that will have its source inventory set.
	 * @param ParentInventory The parent UI Manager to associate with the target widget.
	 * @return Returns true if the source inventory was successfully set; otherwise, returns false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Wrapper", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(Keywords="container,viewport"),
		DisplayName="Set Wrapper Widget Parent Manager")
	static void SetSourceInventory(const TScriptInterface<IMounteaAdvancedBaseInventoryWidgetInterface>& Target, 
		const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& ParentInventory);

	/**
	 * Returns all currently active Widget State tags tracked by the target Wrapper.
	 *
	 * Wrapper widget states are used to keep track of which UI widgets are currently active/visible/in use
	 * within the Wrapper (for example Modal Window, Tooltip, Context Menu, Inventory Panel, etc.).
	 * Each widget that is created/added to the Wrapper should contribute its predefined Gameplay Tag
	 * so the Wrapper and UI Manager can quickly query what is active and react accordingly.
	 *
	 * @param Target The target Wrapper widget interface from which the active state container will be retrieved.
	 * @return Container of Gameplay Tags representing active UI states/widgets currently tracked by the Wrapper.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Wrapper Widget States")
	static FGameplayTagContainer GetWrapperWidgetStates(UObject* Target);

	/**
	 * Overwrites Wrapper widget state tracking with a new tag container on the target Wrapper.
	 *
	 * This replaces the current tracked Widget State tags with the provided container.
	 * Intended for bulk synchronization/reset scenarios (rebuild, restore, switching UI modes).
	 *
	 * Notes:
	 * - This only updates state tracking. It should not be assumed to create/destroy widgets by itself.
	 * - Implementations should ensure the stored state remains consistent with actual active widgets.
	 *
	 * @param Target The target Wrapper widget interface whose states will be overwritten.
	 * @param NewStates Container of Gameplay Tags that will become the Wrapper's active widget states.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Set Wrapper Widget States")
	static void SetWrapperWidgetStates(UObject* Target,
		const FGameplayTagContainer& NewStates);

	/**
	 * Adds a new Widget State tag to the target Wrapper's active state container.
	 *
	 * Expected flow:
	 * - A child widget (e.g., ModalWindow) is created/added to the Wrapper.
	 * - That widget provides its state tag definition.
	 * - Wrapper registers the tag so the system can track that the widget is now active.
	 *
	 * @param Target The target Wrapper widget interface that will receive the state tag.
	 * @param Tag The Gameplay Tag describing the widget state to add.
	 * @return True if the tag was added (was not present before). False if invalid or already present.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Add Wrapper Widget State Tag")
	static bool AddWidgetStateTag(UObject* Target,
		const FGameplayTag& Tag);

	/**
	 * Removes an existing Widget State tag from the target Wrapper's active state container.
	 *
	 * This should be called when a UI element is removed/hidden/destroyed so the Wrapper no longer reports
	 * it as active, preventing stale state and enabling correct UI Manager decisions.
	 *
	 * @param Target The target Wrapper widget interface from which the state tag will be removed.
	 * @param Tag The Gameplay Tag describing the widget state to remove.
	 * @return True if the tag was removed. False if invalid or not found.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Remove Wrapper Widget State Tag")
	static bool RemoveWidgetStateTag(UObject* Target,
		const FGameplayTag& Tag);

	/**
	 * Checks whether the target Wrapper currently contains a given Widget State tag.
	 *
	 * Used for querying whether a particular UI element/state is currently active within the Wrapper.
	 * Example: prevent opening another modal if "UI.Modal.Active" is already present, or change input rules.
	 *
	 * @param Target The target Wrapper widget interface to query.
	 * @param Tag The Gameplay Tag describing the widget state to check.
	 * @param bExactMatch If true, requires an exact tag match. If false, allows hierarchical matching
	 *                    (e.g., checking "UI.Modal" would match "UI.Modal.Active").
	 * @return True if the Wrapper currently reports the tag as active; otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Has Wrapper Widget State Tag")
	static bool HasWidgetStateTag(UObject* Target,
		const FGameplayTag& Tag, bool bExactMatch = true);

	/**
	 * Clears all Widget State tags tracked by the target Wrapper.
	 *
	 * Typically used during full teardown/reset scenarios such as RemoveWrapperWidget, rebuilding the UI,
	 * or when the UI Manager needs to force the Wrapper into a clean baseline state.
	 *
	 * Note:
	 * - This does not automatically destroy UI widgets by itself. It only clears the tracked state tags.
	 *
	 * @param Target The target Wrapper widget interface whose states will be cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Clear Wrapper Widget State Tags")
	static void ClearWidgetStateTags(UObject* Target);

	/**
	 * Appends multiple Widget State tags to the target Wrapper's active state container.
	 *
	 * Useful when adding a composite widget (or UI mode) that activates multiple tracked states at once,
	 * or when synchronizing the Wrapper to a known set of states provided by another system.
	 *
	 * @param Target The target Wrapper widget interface that will receive the state tags.
	 * @param TagsToAppend Container of Gameplay Tags to add to the Wrapper's active state container.
	 * @return True if at least one new tag was added. False if empty or all tags were already present.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Wrapper",
		meta=(CustomTag="MounteaK2Setter"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Append Wrapper Widget State Tags")
	static bool AppendWidgetStateTags(UObject* Target,
		const FGameplayTagContainer& TagsToAppend);

#pragma endregion
	
	// --- Notification
#pragma region Notification
	
	/**
	 * Retrieves the widget that contains and manages inventory notifications.
	 * @param Target The UI interface to get the notification container from
	 * @return A pointer to the user widget that serves as the notification container
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", 
		meta=(CustomTag="MounteaK2Getter"))
	static UUserWidget* GetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Sets the widget that will contain and manage inventory notifications.
	 * @param Target The UI interface to set the notification container for
	 * @param NewNotificationContainer The user widget to use as the notification container
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"))
	static void SetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, UUserWidget* NewNotificationContainer);

	/**
	 * Creates and displays a new inventory notification based on the provided data.
	 * @param Target The UI interface to create the notification in
	 * @param NotificationData The data structure containing all notification parameters (text, duration, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"))
	static void CreateInventoryNotification(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, const FInventoryNotificationData& NotificationData);

	/**
	 * Removes all currently active inventory notifications from the UI.
	 * @param Target The UI interface to remove notifications from
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Notifications", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"))
	static void RemoveInventoryNotifications(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

#pragma endregion
		
	// --- Items Preview
#pragma region ItemsPreview

	/**
	 * Initializes the given Interactable Widget.
	 *
	 * @param Target A pointer to the UMounteaAdvancedInventoryInteractableObjectWidget that needs initialization.
	 * @return True if the Target widget is valid and initialized successfully, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
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
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Clear Preview")
	static void ItemPreview_ClearPreview(UMounteaAdvancedInventoryInteractableObjectWidget* Target);

	/**
	 * Resets the item preview for the specified interactable object widget.
	 *
	 * @param Target The target widget representing an advanced inventory interactable object.
	 * The camera for this widget will be reset to its default state if it is valid.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Reset Preview")
	static void ItemPreview_ResetPreview(UMounteaAdvancedInventoryInteractableObjectWidget* Target);

	/**
	 * Updates camera rotation based on mouse delta input for the specified target widget.
	 *
	 * @param Target The target widget to update camera rotation on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param MouseDelta The mouse movement delta values for rotation calculation.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Rotation (Mouse)")
	static void ItemPreview_UpdateCameraRotation(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2f& MouseDelta);

	/**
	 * Updates camera height based on mouse delta input for the specified target widget.
	 *
	 * @param Target The target widget to update camera height on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param MouseDelta The mouse movement delta values for height calculation.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Height (Mouse)")
	static void ItemPreview_UpdateCameraHeight(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const FVector2f& MouseDelta);

	/**
	 * Updates camera zoom based on mouse wheel input for the specified target widget.
	 *
	 * @param Target The target widget to update camera zoom on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param WheelDelta The mouse wheel delta value for zoom calculation.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
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
	static void ItemPreview_SetCameraRotationAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
		const float YawNormalized, const float PitchNormalized);

	/**
	 * Sets camera height to absolute value for the specified target widget.
	 *
	 * @param Target The target widget to set camera height on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param HeightNormalized The normalized height value (0.0 to 1.0) representing the full height range.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Camera Height Absolute (Slider)")
	static void ItemPreview_SetCameraHeightAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float HeightNormalized);

	/**
	 * Sets camera zoom to absolute value for the specified target widget.
	 *
	 * @param Target The target widget to set camera zoom on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param ZoomNormalized The normalized zoom value (0.0 to 1.0) representing the full zoom range.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Set Camera Zoom Absolute (Slider)")
	static void ItemPreview_SetCameraZoomAbsolute(UMounteaAdvancedInventoryInteractableObjectWidget* Target, const float ZoomNormalized);

	/**
	 * Updates camera rotation based on analog gamepad input for the specified target widget.
	 *
	 * @param Target The target widget to update camera rotation on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param AnalogInput The analog input values (-1.0 to 1.0) from gamepad stick for rotation calculation.
	 * @param DeltaTime The time elapsed since the last frame for frame-rate independent movement.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Rotation Analog (Gamepad)")
	static void ItemPreview_UpdateCameraRotationAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
		const FVector2f& AnalogInput, const float DeltaTime);

	/**
	 * Updates camera height based on analog gamepad input for the specified target widget.
	 *
	 * @param Target The target widget to update camera height on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param AnalogInput The analog input value (-1.0 to 1.0) from gamepad for height calculation.
	 * @param DeltaTime The time elapsed since the last frame for frame-rate independent movement.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Height Analog (Gamepad)")
	static void ItemPreview_UpdateCameraHeightAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
		const float AnalogInput, const float DeltaTime);

	/**
	 * Updates camera zoom based on analog gamepad input for the specified target widget.
	 *
	 * @param Target The target widget to update camera zoom on. This must derive from UMounteaAdvancedInventoryInteractableObjectWidget.
	 * @param AnalogInput The analog input value (-1.0 to 1.0) from gamepad for zoom calculation.
	 * @param DeltaTime The time elapsed since the last frame for frame-rate independent movement.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemPreview", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="ItemPreview - Update Camera Zoom Analog (Gamepad)")
	static void ItemPreview_UpdateCameraZoomAnalog(UMounteaAdvancedInventoryInteractableObjectWidget* Target, 
		const float AnalogInput, const float DeltaTime);
	
#pragma endregion
	
	// --- Item Action
#pragma region ItemActions
	
	/**
	 * Checks if the specified flags are set in the value.
	 * @param Value - The value to check
	 * @param FlagMask - The flag(s) to check for
	 * @return - True if all flags in the mask are set in the value
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Inventory|UI|Item Action", 
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Item Action - Does Have Flag")
	static bool ItemAction_HasFlag(const uint8 Value, 
		UPARAM(meta = (Bitmask, BitmaskEnum="/Script/MounteaAdvancedInventorySystem.EInventoryItemActionCallback")) const int32 FlagMask)
	{
		return (Value & FlagMask) == FlagMask;
	}

	/**
	 * Initializes the item action widget with the Initiator UI and item action data.
	 * 
	 * @param Target Widget object which implements MounteaAdvancedInventoryItemActionWidgetInterface
	 * @param ItemAction The item action associated with this widget.
	 * @param SelectedItem Inventory Item which this action affects.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Action - Initialize")
	static void ItemAction_InitializeItemAction(UWidget* Target,
		const UMounteaSelectableInventoryItemAction* ItemAction, const FGuid& SelectedItem);

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
		DisplayName="Item Action - Is Action Enabled")
	static bool ItemAction_IsActionEnabled(UWidget* Target);

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
		DisplayName="Item Action - Is Action Valid")
	static bool ItemAction_IsActionValid(UWidget* Target);

	/**
	 * Executes the item action associated with the specified target widget.
	 * This function checks if the target widget implements the MounteaAdvancedInventoryItemActionWidgetInterface
	 * and calls the ExecuteItemAction method if available.
	 *
	 * @param Target The target widget from which to execute the item action. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Action - Execute Item Action")
	static void ItemAction_ExecuteItemAction(UWidget* Target);

	/**
	 * Retrieves the item action class associated with the specified target widget.
	 * This function checks if the target widget implements the MounteaAdvancedInventoryItemActionWidgetInterface
	 * and retrieves the item action class if available.
	 *
	 * @param Target The target widget from which to retrieve the item action class. Must implement the MounteaAdvancedInventoryItemActionWidgetInterface.
	 * 
	 * @return A TSoftClassPtr to the UMounteaInventorySimpleItemAction class if available; otherwise, returns nullptr.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Action",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Item Action - Get Item Action")
	static TSoftClassPtr<UMounteaSelectableInventoryItemAction> ItemAction_GetItemAction(UWidget* Target);

#pragma endregion
	
		// --- Item Actions Container
#pragma region ItemActionsContainer
	
	static bool IsValidActionsContainerWidget(const UObject* Target);

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
	static void ItemActionsContainer_ConstructFromActionsList(UWidget* Target, const TArray<UMounteaSelectableInventoryItemAction*>& ItemActionsList);

	/**
	 * Adds an item action widget to the container.
	 * 
	 * @param Target The target widget. Must implement the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemAction The Action that we need to generate widget for.
	 * @param ItemId Item Guid which this Action is related to.
	 * 
	 * @return Returns create Widget if action is successful
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Add Item Action To Container")
	static UWidget* ItemActionsContainer_AddItemActionToContainer(UWidget* Target, UMounteaSelectableInventoryItemAction* ItemAction, const FGuid& ItemId);

	/**
	 * Removes the specified item action widget from the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionWidget The item action widget to be removed from the container.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Remove Item Action From Container")
	static void ItemActionsContainer_RemoveItemActionFromContainer(UWidget* Target, UWidget* ItemActionWidget);

	/**
	 * Clears all item action widgets from the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Clear Item Actions Container")
	static void ItemActionsContainer_ClearItemActionsContainer(UWidget* Target);

	/**
	 * Selects a specific item action widget within the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionWidget The item action widget to be selected.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Select Item Action")
	static void ItemActionsContainer_SelectItemAction(UWidget* Target, UWidget* ItemActionWidget);

	/**
	 * Retrieves all item action widgets currently present in the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @return An array of UWidget pointers representing the item actions in the container.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Item Actions In Container")
	static TArray<UWidget*> ItemActionsContainer_GetItemActionsInContainer(UWidget* Target);

	/**
	 * Selects an item action widget by its index within the item actions container.
	 *
	 * @param Target The target widget that implements the MounteaAdvancedInventoryItemActionsContainerWidgetInterface.
	 * @param ItemActionIndex The index of the item action to be selected.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Item Actions Container",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Get Item Actions In Container By Index")
	static void ItemActionsContainer_SelectItemActionByIndex(UWidget* Target, int32 ItemActionIndex);
	
#pragma endregion
	
	// --- OLD
	
	// --- Theme
#pragma region Theme	

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Theme", 
		meta=(CustomTag="MounteaK2Getter"))
	static UMounteaAdvancedInventoryThemeConfig* GetThemeConfig();

#pragma endregion

	// --- Base Widget
#pragma region BaseWidget

	/**
	 * Sets the reference to the owning inventory UI interface.
	 * This function allows the widget to be associated with a specific inventory UI.
	 *
	 * @param Target The UI interface to create the inventory UI for, must implement `IMounteaAdvancedBaseInventoryWidgetInterface`
	 * @param NewOwningInventoryUI A TScriptInterface reference to the inventory UI interface that owns this widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Base", meta=(CustomTag="MounteaK2Setter"))
	static void SetOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& NewOwningInventoryUI);
	
#pragma endregion
	
	// --- InventoryUI
#pragma region InventoryUI
	
	/**
	 * Triggers the 'CategorySelected' functionality on a given target, indicating that a specific inventory category has been selected.
	 *
	 * @param Target The interface implementing the IMounteaAdvancedInventoryUIManagerInterface, representing the inventory UI where the category selection operation should occur.
	 * @param SelectedCategoryId The identifier of the selected category that needs to be processed by the target.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void CategorySelected(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, const FString& SelectedCategoryId);

	/**
	 * Retrieves the selected category ID from the specified inventory UI target.
	 *
	 * @param Target The inventory UI interface target from which to retrieve the selected category ID.
	 * @return The selected category ID as a string. Returns "none" if the target object is invalid or the operation fails.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static FString GetSelectedCategoryId(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Triggers the ItemSelected action on the specified inventory target.
	 *
	 * @param Target A TScriptInterface referencing the target implementing the IMounteaAdvancedInventoryUIManagerInterface.
	 *               Represents the inventory UI on which the action will be executed.
	 * @param ItemGuid A FGuid that uniquely identifies the item being selected within the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void ItemSelected(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, const FGuid& ItemGuid);

	/**
	 * Retrieves the GUID of the currently selected item from the provided inventory UI interface.
	 *
	 * @param Target A TScriptInterface reference to the inventory UI interface from which the selected item's GUID is retrieved.
	 * @return FGuid of the selected item if the interface is valid; otherwise, returns an empty FGuid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static FGuid GetSelectedItemGuid(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);
	
	/**
	 * Retrieves the currently active item widget in the inventory UI.
	 *
	 * This function returns a pointer to the active item widget currently being displayed or interacted with.
	 *
	 * @param Target 
	 * @return A pointer to the UUserWidget representing the active item widget, or nullptr if no widget is active.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Inventory UI Manager - Get Active Item Widget")
	static UWidget* GetActiveItemWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Sets the active item widget in the UI.
	 *
	 * @param Target 
	 * @param NewActiveItemWidget The new widget to be set as the active item widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Inventory UI Manager - Set Active Item Widget")
	static void SetActiveItemWidget(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, UWidget* NewActiveItemWidget);

#pragma endregion

	// --- Inventory
#pragma region Inventory
	/**
	 * Retrieves the parent inventory associated with this UI.
	 * @param Target The UI interface to get the parent inventory from
	 * @return The parent inventory interface that manages the items and slots
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Getter"))
	static TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target);

	/**
	 * Sets the parent inventory for this UI.
	 * @param Target The UI interface to set the parent inventory on
	 * @param NewParentInventory The inventory interface to associate with this UI
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);

	/**
	 * Assigns an inventory UI owner to the given widget.
	 *
	 * @param Target The widget to which the owning inventory UI will be assigned.
	 * @param OwningInventoryUI The owning inventory UI interface implementation to be set on the target widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Inventory", meta=(CustomTag="MounteaK2Setter"))
	static void SetInventoryOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI);
	
#pragma endregion

	// --- Categories Wrapper Widget
#pragma region CategoriesWrapper

	/**
	 * Associates the specified widget with the provided inventory UI interface.
	 *
	 * @param Target The widget to associate with the inventory UI.
	 * @param OwningInventoryUI The inventory UI interface to associate with the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetCategoriesWrapperOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI);

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
	
	// --- Category Widget
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
	static void SetCategoryOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI);

	/**
	 * Sets the active state of a target widget implementing the MounteaAdvancedInventoryCategoryWidgetInterface.
	 *
	 * @param Target     The widget to update the active state for. It must be valid and implement the required interface.
	 * @param bIsActive  A boolean indicating whether the widget should be set as active (true) or inactive (false).
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories", meta=(CustomTag="MounteaK2Setter"))
	static void SetActiveState(UWidget* Target, const bool bIsActive);
	
#pragma endregion	
	
	// --- Item Widget
#pragma region Item
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Inventory Item Widget Data",
		meta=(NativeMakeFunc))
	static FInventoryItemData MakeInventoryItemWidgetData(const FInventoryItem& Item, const int32 Quantity);

	/**
	 * Validates whether Item Widget Data has changed (Widget is Dirty for re-render).
	 * 
	 * @param NewState New expected state of Data
	 * @param OldState Previous state of Data
	 * @return true if anything changed, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Validate"),
		meta=(Keywords="dirty,check,validate,equal,=="),
		DisplayName="Compare Item Widget Data")
	static bool CompareItemWidgetData(const FInventoryItemData& NewState, const FInventoryItemData& OldState);
	
	/**
	 * Selects this inventory item in the UI.
	 * 
	 * This method checks if the provided widget implements the MounteaAdvancedInventoryItemWidgetInterface and,
	 * if so, calls to the Item Widget to process its selection.
	 * 
	 * @param Target The widget from which the Inventory Item ID is to be retrieved.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item - Select Item")
	static void ItemWidget_SelectItem(UWidget* Target);

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
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Item - Get Item ID")
	static FGuid ItemWidget_GetInventoryItemId(UWidget* Target);

	/**
	 * Sets the owning inventory UI interface for the specified widget.
	 *
	 * @param Target The UWidget that will have its owning inventory UI set.
	 * @param OwningInventoryUI The interface that represents the owning inventory UI to be assigned to the widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI);

	/**
	 * Initializes the item widget with the provided inventory item and quantity.
	 *
	 * Implementations should:
	 * - Store the passed item and quantity internally (e.g. into FInventoryItemData).
	 * - Set up any initial visuals (icon, name, rarity, etc.).
	 * - Optionally call RefreshItemWidget or similar logic to ensure the UI is up to date.
	 *
	 * This is typically called when the widget is first created or when it is
	 * re-bound to a different inventory entry.
	 *
	 * @param Target Inventory Item Widget.
	 * @param Item     Inventory item to be represented by this widget.
	 * @param Quantity Initial quantity of the item to display.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item - Initialize Item Widget")
	static void ItemWidget_InitializeItemWidget(UWidget* Target, const FInventoryItem& Item, const int32 Quantity);
	
	/**
	 * Retrieves the logical data currently represented by this item widget.
	 *
	 * Implementations should always return the latest data that the UI reflects.
	 *
	 * @param Target Inventory Item Widget.
	 * @return A copy of the item’s data structure describing quantity and contained item state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
			meta=(CustomTag="MounteaK2Getter"),
			DisplayName="Item - Get Item Widget Data")
	static FInventoryItemData ItemWidget_GetInventoryData(UWidget* Target);

	/**
	 * Assigns new inventory data to this widget.
	 *
	 * Implementations should update visuals immediately or trigger an internal refresh.
	 * 
	 * @param Target Inventory Item Widget.
	 * @param InventoryItemData Data describing the item and its quantity.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|Items",
			meta=(CustomTag="MounteaK2Setter"),
			DisplayName="Item - Set Item Widget Data")
	static void ItemWidget_SetInventoryData(UWidget* Target, const FInventoryItemData& InventoryItemData);
	
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
		
	// --- Item Tooltip
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
	
	// --- Item Slot Widget
#pragma region ItemSlot

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Slot Data",
		meta=(NativeMakeFunc))
	static FInventorySlot MakeInventoryGridSlotData(const FInventorySlot& SlotData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Grid Slot Data",
		meta=(NativeMakeFunc))
	static FInventorySlot MakeInventorySlotData(const FMounteaInventoryGridSlot& GridSlotData);

	/**
	 * Create new inventory slot widget with the specified parameters.
	 * @param UserWidget The user widget that will contain the inventory slot.
	 * @param ItemId The unique identifier of the item to be displayed in the slot.
	 * @return A new FInventorySlot instance containing the item ID and quantity.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Make New Inventory Slot Data",
		meta=(NativeMakeFunc))
	static FInventorySlot MakeInventorySlot(UUserWidget* UserWidget, const FGuid& ItemId);

	/**
	 * Sets the owning inventory UI for a given item slot widget.
	 *
	 * @param Target The target widget item slot to associate with the owning inventory UI.
	 * @param OwningInventoryUI The inventory UI interface to set as the owner of the target item slot.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Item Slot - Set Item Slot Owning Inventory UI")
	static void ItemSlot_SetItemSlotOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI);

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
	static void ItemSlot_StoreGridSlotData(UWidget* Target, const FMounteaInventoryGridSlot& SlotData);

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
	static FMounteaInventoryGridSlot ItemSlot_GetGridSlotData(UWidget* Target);

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

	// --- Item Slots Wrapper
#pragma region ItemSlotsWrapper

	/**
	 * Sets the owning inventory UI for the specified widget target.
	 *
	 * @param Target The target widget for which the owning inventory UI is being set.
	 * @param OwningInventoryUI The interface of the advanced inventory UI to be set as the owner.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"))
	static void SetItemSlotsWrapperOwningInventoryUI(UWidget* Target, const TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface>& OwningInventoryUI);

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
	static UWidget* SlotsWrapper_GetSelectedItemWidget(UWidget* Target);

	/**
	 * Attempts to set Item Widget as Selected one.
	 *
	 * @param Target The widget that implements the Slots Wrapper.
	 * @param NewSelectedItemWidget The widget to set as the currently selected item widget.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Inventory|UI|ItemSlots",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Slots Wrapper - Set Selected Item Widget")
	static void SlotsWrapper_SetSelectedItemWidget(UWidget* Target, UWidget* NewSelectedItemWidget);
	
#pragma endregion

	// --- Items Grid
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
		TScriptInterface<IMounteaAdvancedInventoryUIManagerInterface> ParentUIComponent);
	
#pragma endregion
	
	// --- Helpers (OLD)
#pragma region Helpers_Old
	
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
};
