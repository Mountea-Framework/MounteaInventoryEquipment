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
#include "MounteaCraftingUIStatics.generated.h"

class UMounteaAdvancedCraftingUIConfig;
class UMounteaAdvancedCraftingUISubsystem;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Retrieves the crafting UI subsystem associated with the provided context.
	 *
	 * This function determines the player controller tied to the given context and returns
	 * the associated Mountea advanced crafting UI subsystem if available.
	 *
	 * @param Context The context object used to locate the player controller. This can be an actor,
	 *                actor component, or user widget. If invalid, the function returns nullptr.
	 * @return A pointer to the UMounteaAdvancedCraftingUISubsystem if found; otherwise, nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Subsystems", 
		meta=(CustomTag="MounteaK2Getter"),
		meta=(DefaultToSelf="Context"),
		meta=(CompactNodeTitle="Crafting UI Subsystem"),
		DisplayName="Get Crafting UI Subsystem")
	static UMounteaAdvancedCraftingUISubsystem* GetCraftingUISubsystem(UObject* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Crafting UI Config")
	static UMounteaAdvancedCraftingUIConfig* GetCraftingUISettingsConfig();
};
