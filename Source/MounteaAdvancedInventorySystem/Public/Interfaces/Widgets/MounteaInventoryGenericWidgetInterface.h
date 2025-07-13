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
#include "UObject/Interface.h"
#include "MounteaInventoryGenericWidgetInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryGenericWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMounteaInventoryGenericWidgetInterface provides base functionality for all inventory UI widgets.
 * Generic widget interfaces define command processing, theme application, and refresh operations
 * for consistent widget behavior across the inventory system.
 *
 * @see [Widget Interfaces](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/UserInterface)
 * @see UMounteaAdvancedInventoryBaseWidget
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaInventoryGenericWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Processes a specific command for the inventory widget with optional additional data.
	 *
	 * This method is intended to handle inventory and/or equipment-related commands for user interface elements.
	 * It can optionally utilize a payload object to provide additional context or data for the command.
	 *
	 * @param Command The command string to process, typically indicating an action or directive.
	 * @param OptionalPayload An optional UObject providing additional context or data. Can be nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|User Interface")
	void ProcessInventoryWidgetCommand(const FString& Command, UObject* OptionalPayload = nullptr);
	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload = nullptr) = 0;

	/**
	 * Applies a visual theme or styling configuration to the widget.
	 *
	 * This method is intended to be overridden in Blueprint or C++ to define the specific
	 * logic for applying themes to inventory or equipment-related user interface elements.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|User Interface")
	void ApplyTheme();
	virtual void ApplyTheme_Implementation() = 0;

	/**
	 * Refreshes the widget to update its visual representation or data.
	 *
	 * This method can be overridden to define specific logic for refreshing
	 * the user interface element associated with inventory or equipment systems.
	 * It may be used for reloading data, updating state, or applying changes made
	 * to the widget’s configuration or content.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|User Interface")
	void RefreshWidget();
	virtual void RefreshWidget_Implementation() = 0;
};
