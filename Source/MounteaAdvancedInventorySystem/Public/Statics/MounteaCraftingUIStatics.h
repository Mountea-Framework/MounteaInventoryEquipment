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

class IMounteaAdvancedCraftingParticipantInterface;
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
		meta=(DisplayName="Crafting UI - Get Crafting UI Subsystem"))
	static UMounteaAdvancedCraftingUISubsystem* GetCraftingUISubsystem(UObject* Context);
		
	/**
	 * Returns the crafting UI settings configuration.
	 *
	 * @return Crafting UI configuration asset, or nullptr when unavailable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config", 
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Crafting UI Config"))
	static UMounteaAdvancedCraftingUIConfig* GetCraftingUISettingsConfig();

	/**
	 * Returns the configured recipe details panel widget class after validating its marker interface.
	 *
	 * @return Widget class implementing UMounteaAdvancedInventoryCraftingRecipeDetailsPanelWidgetInterface, or nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Recipe Details Panel Widget Class"))
	static TSubclassOf<UUserWidget> GetCraftingRecipeDetailsPanelWidgetClass();

	/**
	 * Returns the configured recipe ingredients wrapper widget class after validating its marker interface.
	 *
	 * @return Widget class implementing UMounteaAdvancedInventoryCraftingRecipeIngredientsWrapperWidgetInterface, or nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Recipe Ingredients Wrapper Widget Class"))
	static TSubclassOf<UUserWidget> GetCraftingRecipeIngredientsWrapperWidgetClass();

	/**
	 * Returns the configured recipe ingredient widget class after validating its marker interface.
	 *
	 * @return Widget class implementing UMounteaAdvancedInventoryCraftingRecipeIngredientWidgetInterface, or nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Recipe Ingredient Widget Class"))
	static TSubclassOf<UUserWidget> GetCraftingRecipeIngredientWidgetClass();

	/**
	 * Returns the configured recipe result widget class after validating its marker interface.
	 *
	 * @return Widget class implementing UMounteaAdvancedInventoryCraftingRecipeResultWidgetInterface, or nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Recipe Result Widget Class"))
	static TSubclassOf<UUserWidget> GetCraftingRecipeResultWidgetClass();

	/**
	 * Returns the configured recipe infobox widget class after validating its marker interface.
	 *
	 * @return Widget class implementing UMounteaAdvancedInventoryCraftingRecipeInfoboxWidgetInterface, or nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Recipe Infobox Widget Class"))
	static TSubclassOf<UUserWidget> GetCraftingRecipeInfoboxWidgetClass();

	/**
	 * Returns the configured recipe slider wrapper widget class after validating its marker interface.
	 *
	 * @return Widget class implementing UMounteaAdvancedInventoryCraftingRecipeSliderWidgetInterface, or nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Config",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting UI - Get Recipe Slider Widget Class"))
	static TSubclassOf<UUserWidget> GetCraftingRecipeSliderWidgetClass();
	
	// --- Crafting Participant

	/**
	 * Retrieves the parent crafting participant associated with a specified object.
	 *
	 * This function checks if the given object is valid and implements the
	 * MounteaAdvancedCraftingParticipantUIInterface. If so, it retrieves the parent crafting participant from the object.
	 *
	 * @param Target The object for which to retrieve the parent crafting participant.
	 * @return A TScriptInterface of IMounteaAdvancedCraftingParticipantInterface
	 *         if a parent crafting participant exists, or nullptr if the object is invalid
	 *         or does not implement the required interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager", 
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant UI - Get Parent Crafting Participant Interface"))
	static TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> GetParentCraftingParticipant(const UObject* Target);
	
	/**
	 * Retrieves the parent crafting participant associated with a specified object.
	 *
	 * This function checks if the given object is valid and implements the
	 * MounteaAdvancedCraftingParticipantUIInterface. If so, it retrieves the parent crafting participant from the object.
	 *
	 * @param Target The object for which to retrieve the parent crafting participant.
	 * @return A UObject of IMounteaAdvancedCraftingParticipantInterface
	 *         if a parent crafting participant exists, or nullptr if the object is invalid
	 *         or does not implement the required interface.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|UI|Manager", 
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant UI - Get Parent Crafting Participant"))
	static UObject* GetParentCraftingParticipantObject(const UObject* Target);

	/**
	 * Attempts to associate a crafting participant with a target object that implements the required interface.
	 *
	 * This function verifies the validity of the provided target object and checks if it implements the
	 * UMounteaAdvancedCraftingParticipantUIInterface. If the target is valid and compatible, it sets the crafting participant for the target object.
	 *
	 * @param Target The object to which the crafting participant will be assigned. Must implement UMounteaAdvancedCraftingParticipantUIInterface.
	 * @param Participant The crafting participant to be associated with the target.
	 * @return True if the crafting participant was successfully set; otherwise, false.
	 */
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|UI|Manager",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant UI - Set Parent Crafting Participant"))
	static bool SetParentCraftingParticipant(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);
	
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
		meta=(DisplayName="Crafting Participant UI - Create Crafting Widget"))
	static bool CreateCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target);
	
	/**
	 * Returns the currently active Crafting widget.
	 *
	 * @return Crafting widget if available, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting Participant UI - Get Crafting Widget"))
	static UUserWidget* GetCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target);

	/**
	 * Sets the Crafting UI from outside.
	 * @return True if UI was successfully set, otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant UI - Set Crafting Widget"))
	static bool SetCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target, UUserWidget* NewCraftingWidget);

	/**
	 * Removes and cleans up the Crafting widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		meta=(DisplayName="Crafting Participant UI - Remove Crafting Widget"))
	static void RemoveCraftingWidget(const TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface>& Target);

	/**
	 * Retrieves the currently selected Crafting category ID.
	 * 
	 * @param Target The Crafting Participant UI object to retrieve the activecategory from.
	 * @return The ID of the currently selected Crafting category.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting Participant UI - Get Selected Crafting Category"))
	static FString GetCraftingSelectedCategoryId(UObject* Target);
	
	/**
	 * Handles the selection of a Crafting category in the Crafting UI.
	 * 
	 * @param Target The Crafting Participant UI object.
	 * @param SelectedCategoryId The ID of the selected category.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		meta=(DisplayName="Crafting Participant UI - Select Crafting Category"))
	static bool CraftingCategorySelected(UObject* Target, const FString& SelectedCategoryId);

	/**
	 * Retrieves the currently active Craftable item GUID.
	 *
	 * @param Target The Crafting Participan tUI object to retrieve the active craftable item's GUID from.
	 * @return  The GUID of the currently active Craftable item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting Participant UI - Get Active Craftable Item Guid"),
		meta=(Keywords="active,selected,recipe"))
	static FGuid GetActiveCraftableItemGuid(UObject* Target);
	
	/**
	 * Handles the selection of a Craftable item in the Crafting UI.
	 * 
	 * @param Target The Crafting Participant UI object.
	 * @param SelectedItem The GUID of the selected Craftable item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|UI",
		meta=(MounteaSetter),
		meta=(DisplayName="Crafting Participant UI - Select Craftable Item"))
	static bool CraftableItemSelected(UObject* Target, const FGuid& SelectedItem);

private:

	static TSubclassOf<UUserWidget> ResolveCraftingWidgetClass(const TSoftClassPtr<UUserWidget>& WidgetClassPtr, const UClass* RequiredInterface);
};
