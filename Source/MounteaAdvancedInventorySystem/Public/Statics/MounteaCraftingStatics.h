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
#include "Definitions/MounteaCraftingBaseDataTypes.h"
#include "Definitions/MounteaInventoryBaseDataTypes.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaCraftingStatics.generated.h"

class UMounteaRecipeTemplate;
class UMounteaRecipeIngredientsList;
class UMounteaRecipeIngredient;
class UMounteaInventoryItemTemplate;
class UMounteaInventoryItemTemplate_Recipe;
class IMounteaAdvancedCraftingParticipantInterface;
class IMounteaAdvancedCraftingStationInterface;
class IMounteaAdvancedInventoryInterface;

struct FGameplayTag;
struct FMounteaCraftingResult;
struct FMounteaCraftingRecipeSearchFilter;
enum class ECraftingStationState : uint8;

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

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Bind On Crafting Finished")
	static bool BindToOnCraftingFinished(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaCraftingFinishedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Unbind From On Crafting Finished")
	static bool UnbindFromOnCraftingFinished(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaCraftingFinishedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Bind On Recipe Learned")
	static bool BindToOnRecipeLearned(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeLearnedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Unbind From On Recipe Learned")
	static bool UnbindFromOnRecipeLearned(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeLearnedBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Bind On Recipe Forgotten")
	static bool BindToOnRecipeForgotten(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeForgottenBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Unbind From On Recipe Forgotten")
	static bool UnbindFromOnRecipeForgotten(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeForgottenBinding& Binding);
	
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		DisplayName="Does Have Any Recipes")
	static bool DoesHaveAnyRecipes(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(AutoCreateRefTerm="SearchFilter"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get All Available Recipes")
	static TArray<UMounteaRecipeTemplate*> GetFilteredRecipes(UObject* Target, const FMounteaCraftingRecipeSearchFilter& SearchFilter);

	/**
	 * Returns all recipes known by the crafting participant where the craftable item would be in specified Category.
	 * @param Target Crafting participant to query for known recipes.
	 * @param CategoryTag Category to search by.
	 * @return Filtered list of available recipes by single category.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter, MounteaInventoryCategoryPin="CategoryTag"),
		meta=(AutoCreateRefTerm="CategoryTag"),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get All Available Recipes By Category")
	static TArray<UMounteaRecipeTemplate*> GetFilteredRecipesByCategory(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target,
		UPARAM(meta=(Categories="Mountea_Inventory.Categories,Mountea_Inventory.Category,Categories,Category")) const FGameplayTag& CategoryTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Categories With Craftbale Items")
	static TArray<FInventoryCategory> GetAllowedCategoriesWithCraftableItems(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get All Known Recipes")
	static TArray<UMounteaRecipeTemplate*> GetKnownRecipes(UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="RecipeGuid"),
		DisplayName="Get Recipe")
	static UMounteaRecipeTemplate* GetRecipe(UObject* Target, const FGuid& RecipeGuid);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="CraftingStationType"),
		DisplayName="Get Recipes")
	static TArray<UMounteaRecipeTemplate*> GetRecipes(UObject* Target, const FGameplayTag& CraftingStationType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		DisplayName="Is Recipe Known")
	static bool IsRecipeKnown(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Learn Recipe")
	static bool LearnRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Forget Recipe")
	static bool ForgetRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		DisplayName="Is Crafting Possible")
	static bool IsCraftingPossible(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Start Crafting")
	static FMounteaCraftingResult StartCrafting(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Parent Inventory")
	static TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory(UObject* Target);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Set Parent Inventory")
	static bool SetParentInventory(UObject* Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Start Using Crafting Station")
	static bool StartUsingCraftingStation(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Stop Using Crafting Station")
	static bool StopUsingCraftingStation(UObject* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Crafting Station")
	static TScriptInterface<IMounteaAdvancedCraftingStationInterface> GetCraftingStation(UObject* Target);

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		DisplayName="Get All Recipe Templates")
	static TSet<UMounteaRecipeTemplate*> GetAllRecipeTemplates();

	static FMounteaCraftingResult CraftItem(const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Target, const UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);
	
	static bool HasInventoryItemForRecipeSourceCached(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		UMounteaInventoryItemTemplate_Recipe* RecipeSource,
		TMap<UMounteaInventoryItemTemplate_Recipe*, bool>& RecipeSourcePresenceCache);
	
	static TArray<UMounteaRecipeTemplate*> ApplyRecipeSourceGrantFilter(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const TArray<UMounteaRecipeTemplate*>& SourceRecipes);

	static int32 GetIngredientQuantityCached(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		UMounteaInventoryItemTemplate* IngredientTemplate,
		TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache);

	static bool IsIngredientSatisfied(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const UMounteaRecipeIngredient* Ingredient,
		TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache);

	static bool IsIngredientGroupSatisfied(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const UMounteaRecipeIngredientsList* IngredientGroup,
		TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache);

	static bool IsRecipeSatisfiedByIngredients(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const UMounteaRecipeTemplate* Recipe,
		TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache);

	static TArray<UMounteaRecipeTemplate*> ApplyIngredientAvailabilityFilter(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const TArray<UMounteaRecipeTemplate*>& SourceRecipes);

	static bool IsRecipeInCategory(
		const UMounteaRecipeTemplate* Recipe,
		const FGameplayTag& CategoryTag);

	static TArray<UMounteaRecipeTemplate*> ApplyCategoryFilter(
		const TArray<UMounteaRecipeTemplate*>& SourceRecipes,
		const FGameplayTag& CategoryTag);

	static void GetAllowedInventoryCategoryTags(TArray<FGameplayTag>& OutCategoryTags);

	static bool IsAllowedInventoryCategoryTag(const FGameplayTag& CategoryTag);

	static TArray<UMounteaRecipeTemplate*> ApplyStationTypeFilter(
		const TArray<UMounteaRecipeTemplate*>& SourceRecipes,
		const FGameplayTag& StationType);

	static TArray<UMounteaRecipeTemplate*> ApplyRecipeSourceFilter(
		const TArray<UMounteaRecipeTemplate*>& SourceRecipes,
		const UMounteaRecipeTemplate* RecipeSource);

/**
 * CRAFTING PLACE
 */

public:

	static bool IsValidCraftingPlace(const UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Crafting Place Type")
	static FGameplayTag GetCraftingPlaceType(UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		DisplayName="Is Crafting Place Occupied")
	static bool IsCraftingPlaceOccupied(UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Crafting Place Capacity")
	static int32 GetCraftingPlaceCapacity(UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		DisplayName="Can Be Used")
	static bool CanBeUsed(UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Get Crafting Station State")
	static ECraftingStationState GetCraftingStationState(UObject* Target);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="NewState"),
		DisplayName="Set Crafting Station State")
	static bool SetCraftingStationState(UObject* Target, ECraftingStationState NewState);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Start Using Station")
	static bool StartUsing(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);

	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		DisplayName="Stop Using Station")
	static bool StopUsing(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);
	
/**
 * HELPERS
 */

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Helpers",
		meta=(MounteaGetter, MounteaInventoryCategoryPin="CategoryTag"),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="CategoryTag"),
		DisplayName="Get Recipes By Category")
	static TArray<UMounteaRecipeTemplate*> GetRecipesByCategory(
		UObject* Target,
		UPARAM(meta=(Categories="Mountea_Inventory.Categories,Mountea_Inventory.Category,Categories,Category")) const FGameplayTag& CategoryTag);
};
