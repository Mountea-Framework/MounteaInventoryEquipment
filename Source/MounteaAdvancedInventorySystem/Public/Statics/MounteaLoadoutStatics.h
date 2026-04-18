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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaLoadoutStatics.generated.h"

class UMounteaAdvancedInventoryLoadoutConfig;
class UMounteaAdvancedInventoryLoadoutItem;

/**
 * Blueprint helper library for loadout interface operations.
 * Wraps IMounteaAdvancedInventoryLoadoutsInterface calls with safety checks and
 * exposes a simple entry point for gameplay and UI scripts.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaLoadoutStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Validates whether a target object supports the loadouts interface.
	 *
	 * @param Target Object to validate.
	 * @return True if target is valid and implements IMounteaAdvancedInventoryLoadoutsInterface.
	 */
	static bool IsValidLoadout(const UObject* Target);
	
	/**
	 * Executes loadout processing on the target.
	 *
	 * @param Target Object implementing loadouts interface.
	 * @return True if loadout execution succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Loadout",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Load Loadout")
	static bool LoadLoadout(UObject* Target);
	
	/**
	 * Returns loadout configuration currently used by the target.
	 *
	 * @param Target Object implementing loadouts interface.
	 * @return Loadout configuration asset, or nullptr if unavailable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Loadout",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Loadout")
	static UMounteaAdvancedInventoryLoadoutConfig* GetLoadout(UObject* Target);
	
	/**
	 * Returns all loadout entries from the target.
	 *
	 * @param Target Object implementing loadouts interface.
	 * @return Array of loadout items, or empty array when target/configuration is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Loadout",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Loadout Items")
	static TArray<UMounteaAdvancedInventoryLoadoutItem*> GetLoadoutItems(UObject* Target);
	
	/**
	 * Checks whether target is configured for automatic loadout execution.
	 *
	 * @param Target Object implementing loadouts interface.
	 * @return True if automatic load is enabled.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Loadout",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Should Load Automatically")
	static bool ShouldLoadAutomatically(UObject* Target);
	
	/**
	 * Updates automatic load behavior on the target.
	 *
	 * @param Target Object implementing loadouts interface.
	 * @param bNewValue New automatic load flag value.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Loadout",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Set Load Automatically")
	static void SetAutomaticLoad(UObject* Target, const bool bNewValue);
};
