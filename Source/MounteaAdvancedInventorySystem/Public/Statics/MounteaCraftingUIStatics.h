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

class IMounteaAdvancedCraftingParticipantUIInterface;
class UMounteaAdvancedCraftingUIConfig;
class UMounteaAdvancedCraftingUISubsystem;
class UUserWidget;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static bool IsValidCraftingUIHandler(const UObject* Target);

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
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Context"),
		meta=(CompactNodeTitle="Crafting UI Subsystem"),
		DisplayName="Get Crafting UI Subsystem")
	static UMounteaAdvancedCraftingUISubsystem* GetCraftingUISubsystem(UObject* Context);
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config", 
		meta=(MounteaGetter),
		DisplayName="Get Crafting UI Config")
	static UMounteaAdvancedCraftingUIConfig* GetCraftingUISettingsConfig();
	
/**
* CRAFTING UI
*/
	
	/**
	 * Creates and initializes the Crafting widget inside the wrapper.
	 *
	 * @return True if the Crafting widget was created successfully, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Create Crafting Widget")
	static bool CreateCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target);
	
	/**
	 * Returns the currently active Crafting widget.
	 *
	 * @return Crafting widget if available, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaGetter),
		DisplayName="Get Crafting Widget")
	static UUserWidget* GetCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target);

	/**
	 * Sets the Crafting UI from outside.
	 * @return True if UI was successfully set, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Set Crafting Widget")
	static bool SetCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target, UUserWidget* NewCraftingWidget);

	/**
	 * Removes and cleans up the Crafting widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		DisplayName="Remove Crafting Widget")
	static void RemoveCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target);

	/**
	 * Retrieves the currently selected Crafting category ID.
	 * 
	 * @param Target The Crafting Participant UI object to retrieve the activecategory from.
	 * @return The ID of the currently selected Crafting category.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaGetter),
		DisplayName="Get Selected Crafting Category")
	static FString GetCraftingSelectedCategoryId(UObject* Target);
	
	/**
	 * Handles the selection of a Crafting category in the Crafting UI.
	 * 
	 * @param Target The Crafting Participant UI object.
	 * @param SelectedCategoryId The ID of the selected category.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		DisplayName="Select Crafting Category")
	static bool CraftingCategorySelected(UObject* Target, const FString& SelectedCategoryId);

	/**
	 * Retrieves the currently active Craftable item GUID.
	 *
	 * @param Target The Crafting Participan tUI object to retrieve the active craftable item's GUID from.
	 * @return  The GUID of the currently active Craftable item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaGetter),
		DisplayName="Get Active Craftable Item Guid")
	static FGuid GetActiveCraftableItemGuid_Implementation(UObject* Target);
	
	/**
	 * Handles the selection of a Craftable item in the Crafting UI.
	 * 
	 * @param Target The Crafting Participant UI object.
	 * @param SelectedItem The GUID of the selected Craftable item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		DisplayName="Select Craftable Item")
	static bool CraftableItemSelected(UObject* Target, const FGuid& SelectedItem);
};
