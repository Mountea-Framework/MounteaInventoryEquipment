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
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaInventoryGenericWidgetStatics.generated.h"

class UWidget;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryGenericWidgetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static bool IsValidGenericWidget(const UObject* Target);
	
	/**
	 * Delegates command with optional payload to provided Generic Widget.
	 * This allows sending logic commands across multiple levels and through multiple systems.
	 * Example:
	 * - Player picks up item
	 * - Item is added to Inventory (database)
	 * - Inventory (database) calls event OnItemAdded
	 * - Inventory UI Manager calls to Inventory Widget command:
	 * - - RefreshInventory
	 * 
	 * This way Widgets can always know what to do without need to create class-specific logic.
	 * What would be great in near future:
	 * - Create queue system to receive commands
	 * - - Process queue command by command
	 * - - This would solve issues with racing conditions
	 * 
	 * @param GenericWidget Object which must implement MounteaInventoryGenericWidgetInterface interface. For World Space widgets it can be even actor or component.
	 * @param Command String command which defines what to do, eg.: CreateWidget, RefreshWidget, ProcessInput etc.
	 * @param OptionalPayload Object-based optional payloads which can hold additional data around.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", 
		meta=(CustomTag="MounteaK2Setter,MounteaK2Command"),
		meta=(DefaultToSelf="GenericWidget"),
		meta=(Keywords="execute"),
		DisplayName="Process Widget Command")
	static void ProcessInventoryWidgetCommand(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryGenericWidgetInterface")) UObject* GenericWidget, 
		const FString& Command, UObject* OptionalPayload);
	
	/**
	 * Consumes player input forwarded from gameplay classes.
	 *
	 * This method allows Player Controllers or Pawns to route Enhanced Input
	 * actions into UI widgets without exposing input mapping logic to the UI layer.
	 *
	 * Input meaning is conveyed via Gameplay Tags, while values are provided
	 * through a generic payload structure.
	 *
	 * @param Target The UUserWidget to be refreshed. Must be valid and implement UMounteaInventoryGenericWidgetInterface.
	 * @param InputTag A gameplay tag identifying the semantic meaning of the input (e.g. UI.ItemPreview.Zoom, UI.Inventory.Navigate).
	 * @param Payload A lightweight container holding the relevant input value.
	 * @param DeltaTime Frame delta time, used for frame-rate independent behavior.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Consume UI Input")
	static void ConsumeUIInput(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryGenericWidgetInterface")) UWidget* Target, 
		UPARAM(meta=(Categories="Input,Mountea_Inventory.Input")) const FGameplayTag InputTag, 
		const FMounteaWidgetInputPayload& Payload, float DeltaTime);

	/**
	 * Refreshes the provided UserWidget if it implements the MounteaInventoryGenericWidgetInterface.
	 *
	 * This utility function checks if the passed UUserWidget instance is valid and whether
	 * it implements the UMounteaInventoryGenericWidgetInterface. If both conditions are
	 * satisfied, it triggers the execution of the RefreshWidget function on the Target widget.
	 *
	 * @param Target The UUserWidget to be refreshed. Must be valid and implement UMounteaInventoryGenericWidgetInterface.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Refresh Widget")
	static void RefreshWidget(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryGenericWidgetInterface")) UWidget* Target);
	
	/**
	 * Applies the specified theme to the given user widget.
	 * This function ensures that the provided widget is valid and executes the ApplyTheme logic via the Mountea Inventory Generic Widget Interface.
	 *
	 * @param Target The user widget to which the theme will be applied. Must be a valid object.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Apply Widget Theme")
	static void ApplyTheme(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryGenericWidgetInterface")) UWidget* Target);
	
	/**
	 * Sets the Widget Tag definition for this UI element.
	 *
	 * Each UI element that can be hosted/managed by the Wrapper should define a unique Gameplay Tag
	 * representing its identity/state (for example UI.ModalWindow, UI.Tooltip, UI.ContextMenu, etc.).
	 * This tag is used by the Wrapper to track which widgets are currently active and in use, allowing
	 * the UI Manager/Wrapper to:
	 * - prevent duplicate widget instances,
	 * - resolve UI stacking rules (e.g., only one modal at a time),
	 * - adjust input routing (e.g., block interaction while modal is present),
	 * - query current UI composition for debugging/logic.
	 *
	 * Expected usage:
	 * - Widget tag is assigned during initialization / construction of the widget.
	 * - Wrapper reads this tag when the widget is added/removed and updates its internal state tracking.
	 *
	 * @param Target The user widget to set Tag on.
	 * @param WidgetTag Gameplay Tag defining the widget identity/state used for Wrapper/UI tracking.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", 
		meta=(CustomTag="MounteaK2Setter"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Set Widget Tag")
	static void SetWidgetTag(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryGenericWidgetInterface")) UWidget* Target,
		UPARAM(meta=(Categories="Mountea_Inventory.WidgetState,State")) const FGameplayTag& WidgetTag);
	
	/**
	 * Returns the Widget Tag definition for this UI element.
	 *
	 * Each UI element that can be hosted/managed by the Wrapper should expose a Gameplay Tag representing
	 * its identity/state (for example UI.ModalWindow, UI.Tooltip, UI.ContextMenu, etc.).
	 * The Wrapper and UI Manager use this tag to track which widgets are currently active and in use, enabling:
	 * - fast queries of active UI states,
	 * - prevention of duplicate widgets,
	 * - correct UI stacking/layering logic,
	 * - input routing rules based on active UI layers.
	 *
	 * The returned tag is expected to remain stable for the lifetime of the widget instance and should be
	 * assigned via SetWidgetTag during initialization / widget creation.
	 *
	 * @param Target The user widget to get Tag from.
	 * @return Gameplay Tag defining this widget identity/state used for Wrapper/UI tracking.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Generic", 
		meta=(CustomTag="MounteaK2Getter"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Widget Tag")
	static FGameplayTag GetWidgetTag(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryGenericWidgetInterface")) UWidget* Target);
};
