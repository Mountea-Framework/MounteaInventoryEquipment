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
#include "MounteaAdvancedCraftingHandlerInterface.generated.h"

struct FGameplayTag;

class UMounteaRecipeTemplate;
class UMounteaRecipeIngredientsList;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingFinished, const FMounteaCraftingResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeLearned, const UMounteaRecipeTemplate*, RecipeTemplate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeForgotten, const UMounteaRecipeTemplate*, RecipeTemplate);

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedCraftingHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedCraftingHandlerInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Crafting")
	TSet<UMounteaRecipeTemplate*> GetKnownRecipes() const;
	virtual TSet<UMounteaRecipeTemplate*> GetKnownRecipes_Implementation() const = 0;
	
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
	
public:
	
	virtual FOnCraftingFinished& GetOnCraftingFinishedEventHandle() = 0;
	virtual FOnRecipeLearned& GetOnRecipeLearnedEventHandle() = 0;
	virtual FOnRecipeForgotten& GetOnRecipeForgottenEventHandle() = 0;
};
