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
#include "MounteaCraftingStatics.generated.h"

class UMounteaRecipeTemplate;
class UMounteaRecipeIngredientsList;
class IMounteaAdvancedCraftingParticipantInterface;
class IMounteaAdvancedInventoryInterface;

struct FGameplayTag;
struct FMounteaCraftingResult;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
/**
 * CRAFTING PARTICIPANT
 */
	
public:
	
	static bool IsValidRecipeHandler(const UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get All Known Recipes")
	static TArray<UMounteaRecipeTemplate*> GetKnownRecipes(UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Recipe")
	static UMounteaRecipeTemplate* GetRecipe(UObject* Target, const FGuid& RecipeGuid);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Recipes")
	static TArray<UMounteaRecipeTemplate*> GetRecipes(UObject* Target, const FGameplayTag& CraftingStationType);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Is Recipe Known")
	static bool IsRecipeKnown(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Learn Recipe")
	static bool LearnRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Forget Recipe")
	static bool ForgetRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Is Crafting Possible")
	static bool IsCraftingPossible(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Start Crafting")
	static FMounteaCraftingResult StartCrafting(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Is Crafting Possible")
	static TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation(UObject* Target);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Start Crafting")
	static bool SetParentInventory_Implementation(UObject* Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get All Recipe Templates")
	static TSet<UMounteaRecipeTemplate*> GetAllRecipeTemplates();
	
	static FMounteaCraftingResult CraftItem(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
	
/**
 * CRAFTING PLACE
 */
	
public:
	
	static bool IsValidCraftingPlace(const UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Crafting Place Type")
	static FGameplayTag GetCraftingPlaceType(UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(CustomTag="MounteaK2Validate"),
		DisplayName="Is Crafting Place Occupied")
	static bool IsCraftingPlaceOccupied(UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Crafting Place Capacity")
	static int32 GetCraftingPlaceCapacity(UObject* Target);
};
