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
#include "MounteaAdvancedCraftingParticipantUIInterface.generated.h"

class IMounteaAdvancedCraftingParticipantInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedCraftingParticipantUIInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedCraftingParticipantUIInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Creates and initializes the Crafting widget inside the wrapper.
	 *
	 * @return True if the Crafting widget was created successfully, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	bool CreateCraftingWidget();
	virtual bool CreateCraftingWidget_Implementation() = 0;

	/**
	 * Returns the currently active Crafting widget.
	 *
	 * @return Crafting widget if available, otherwise nullptr.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	UUserWidget* GetCraftingWidget() const;
	virtual UUserWidget* GetCraftingWidget_Implementation() const = 0;
	
	/**
	 * Sets the Crafting UI from outside.
	 * @return True if UI was successfully set, otherwise false.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	bool SetCraftingWidget(UUserWidget* NewCraftingWidget);
	virtual bool SetCraftingWidget_Implementation(UUserWidget* NewCraftingWidget) = 0;

	/**
	 * Removes and cleans up the Crafting widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	void RemoveCraftingWidget();
	virtual void RemoveCraftingWidget_Implementation() = 0;
	
	/**
	 * Handles the selection of a Crafting category in the Crafting UI.
	 * @param SelectedCategoryId The ID of the selected category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	bool CraftingCategorySelected(const FString& SelectedCategoryId);
	virtual bool CraftingCategorySelected_Implementation(const FString& SelectedCategoryId) = 0;

	/**
	 * Retrieves the currently selected Crafting category ID.
	 * @return The ID of the currently selected Crafting category.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	FString GetCraftingSelectedCategoryId() const;
	virtual FString GetCraftingSelectedCategoryId_Implementation() const = 0;
	
	/**
	 * Handles the selection of a Craftable item in the Crafting UI.
	 * @param SelectedItem The GUID of the selected Craftable item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	bool CraftableItemSelected(const FGuid& SelectedItem);
	virtual bool CraftableItemSelected_Implementation(const FGuid& SelectedItem) = 0;

	/**
	 * Retrieves the currently active Craftable item GUID.
	 * @return The GUID of the currently active Craftable item.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	FGuid GetActiveCraftableItemGuid() const;
	virtual FGuid GetActiveCraftableItemGuid_Implementation() const = 0;

	/**
	 * Returns Parent Crafting Participant which owns this UI. As UI is always used by Player only, 
	 * the Player Pawn must provide Crafting Participant directly or by using component.
	 * 
	 * @return Returns Parent Crafting Participant which owns this UI or nullptr
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> GetParentCraftingParticipant() const;
	virtual TScriptInterface<IMounteaAdvancedCraftingParticipantInterface> GetParentCraftingParticipant_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|UI|Manager|Crafting")
	bool SetCraftingParticipant(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);
	virtual bool SetCraftingParticipant_Implementation(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant) = 0;
};
