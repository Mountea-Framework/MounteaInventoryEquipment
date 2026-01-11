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

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryGenericWidgetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * Delegates command with optional payload to provided Generic Widget.
	 * This allows sending logic commands across multiple levels and through multiple systems.
	 * Example:
	 * * Player picks up item
	 * * Item is added to Inventory (database)
	 * * Inventory (database) calls event OnItemAdded
	 * * Inventory UI Manager calls to Inventory Widget command:
	 * * * RefreshInventory
	 * 
	 * This way Widgets can always know what to do without need to create class-specific logic.
	 * What would be great in near future:
	 * * Create queue system to receive commands
	 * * * Process queue command by command
	 * * * This would solve issues with racing conditions
	 * 
	 * @param GenericWidget Object which must implement MounteaInventoryGenericWidgetInterface interface. For World Space widgets it can be even actor or component.
	 * @param Command String command which defines what to do, eg.: CreateWidget, RefreshWidget, ProcessInput etc.
	 * @param OptionalPayload Object-based optional payloads which can hold additional data around.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Notification", 
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Process Widget Command")
	static void ProcessInventoryWidgetCommand( UObject* GenericWidget, const FString& Command, UObject* OptionalPayload); 
};
