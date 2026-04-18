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
#include "MounteaAdvancedCraftingParticipantInterface.generated.h"

class IMounteaAdvancedCraftingStationInterface;
class IMounteaAdvancedInventoryInterface;
struct FGameplayTag;
struct FMounteaCraftingResult;

class UMounteaRecipeTemplate;
class UMounteaRecipeIngredientsList;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingFinished, const FMounteaCraftingResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeLearned, const UMounteaRecipeTemplate*, RecipeTemplate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeForgotten, const UMounteaRecipeTemplate*, RecipeTemplate);

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedCraftingParticipantInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedCraftingParticipantInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	TArray<UMounteaRecipeTemplate*> GetKnownRecipes() const;
	virtual TArray<UMounteaRecipeTemplate*> GetKnownRecipes_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	TArray<UMounteaRecipeTemplate*> GetRecipes(const FGameplayTag& CraftingStationType) const;
	virtual TArray<UMounteaRecipeTemplate*> GetRecipes_Implementation(const FGameplayTag& CraftingStationType) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	UMounteaRecipeTemplate* GetRecipe(const FGuid& RecipeGuid) const;
	virtual UMounteaRecipeTemplate* GetRecipe_Implementation(const FGuid& RecipeGuid) const = 0;

	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool IsRecipeKnown(UMounteaRecipeTemplate* RecipeTemplate) const;
	virtual bool IsRecipeKnown_Implementation(UMounteaRecipeTemplate* RecipeTemplate) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool LearnRecipe(UMounteaRecipeTemplate* RecipeTemplate);
	virtual bool LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool ForgetRecipe(UMounteaRecipeTemplate* RecipeTemplate);
	virtual bool ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool IsCraftingPossible(UMounteaRecipeTemplate* TemplateToCraft) const;
	virtual bool IsCraftingPossible_Implementation(UMounteaRecipeTemplate* TemplateToCraft) const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	FMounteaCraftingResult StartCrafting(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
	virtual FMounteaCraftingResult StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory() const;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool SetParentInventory(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);
	virtual bool SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool StartUsingCraftingStation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station);
	virtual bool StartUsingCraftingStation_Implementation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	bool StopUsingCraftingStation();
	virtual bool StopUsingCraftingStation_Implementation() = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	TScriptInterface<IMounteaAdvancedCraftingStationInterface> GetCraftingStation() const;
	virtual TScriptInterface<IMounteaAdvancedCraftingStationInterface> GetCraftingStation_Implementation() const = 0;
	
public:
	
	virtual FOnCraftingFinished& GetOnCraftingFinishedEventHandle() = 0;
	virtual FOnRecipeLearned& GetOnRecipeLearnedEventHandle() = 0;
	virtual FOnRecipeForgotten& GetOnRecipeForgottenEventHandle() = 0;
};
