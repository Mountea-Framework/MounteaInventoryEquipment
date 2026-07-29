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

	/**
	 * Binds a dynamic delegate to the participant's crafting finished event.
	 *
	 * @param Target Crafting participant to bind to.
	 * @param Binding Delegate to add to the crafting finished event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant - Bind On Crafting Finished"))
	static bool BindToOnCraftingFinished(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaCraftingFinishedBinding& Binding);

	/**
	 * Removes a dynamic delegate from the participant's crafting finished event.
	 *
	 * @param Target Crafting participant to unbind from.
	 * @param Binding Delegate to remove from the crafting finished event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant - Unbind From On Crafting Finished"))
	static bool UnbindFromOnCraftingFinished(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaCraftingFinishedBinding& Binding);

	/**
	 * Binds a dynamic delegate to the participant's recipe learned event.
	 *
	 * @param Target Crafting participant to bind to.
	 * @param Binding Delegate to add to the recipe learned event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant - Bind On Recipe Learned"))
	static bool BindToOnRecipeLearned(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeLearnedBinding& Binding);

	/**
	 * Removes a dynamic delegate from the participant's recipe learned event.
	 *
	 * @param Target Crafting participant to unbind from.
	 * @param Binding Delegate to remove from the recipe learned event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Unbind From On Recipe Learned"))
	static bool UnbindFromOnRecipeLearned(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeLearnedBinding& Binding);

	/**
	 * Binds a dynamic delegate to the participant's recipe forgotten event.
	 *
	 * @param Target Crafting participant to bind to.
	 * @param Binding Delegate to add to the recipe forgotten event.
	 * @return True if the binding was added successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Bind On Recipe Forgotten"))
	static bool BindToOnRecipeForgotten(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeForgottenBinding& Binding);

	/**
	 * Removes a dynamic delegate from the participant's recipe forgotten event.
	 *
	 * @param Target Crafting participant to unbind from.
	 * @param Binding Delegate to remove from the recipe forgotten event.
	 * @return True if the binding was removed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Bindings",
		meta=(MounteaBinding),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Unbind From On Recipe Forgotten"))
	static bool UnbindFromOnRecipeForgotten(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target, const FMounteaRecipeForgottenBinding& Binding);
	
	/**
	 * Checks whether the crafting participant knows at least one recipe.
	 *
	 * @param Target Crafting participant to query.
	 * @return True if the participant has any known recipes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant - Does Have Any Recipes"))
	static bool DoesHaveAnyRecipes(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target);
	
	/**
	 * Returns recipes available to the crafting participant after applying the supplied search filter.
	 *
	 * @param Target Crafting participant to query for recipes.
	 * @param SearchFilter Filter settings for ingredients, category, station type, and recipe source.
	 * @return Filtered list of available recipes.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(AutoCreateRefTerm="SearchFilter"),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Get All Available Recipes"))
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
		meta=(DisplayName="Crafting Participant - Get All Available Recipes By Category"))
	static TArray<UMounteaRecipeTemplate*> GetFilteredRecipesByCategory(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target,
		UPARAM(meta=(Categories="Mountea_Inventory.Categories,Mountea_Inventory.Category,Categories,Category")) const FGameplayTag& CategoryTag);

	/**
	 * Returns inventory categories that currently contain craftable recipe result items.
	 *
	 * @param Target Crafting participant to query.
	 * @return Categories with at least one craftable recipe result item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Get Categories With Craftable Items"))
	static TArray<FInventoryCategory> GetAllowedCategoriesWithCraftableItems(UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* Target);

	/**
	 * Returns all recipes known by the crafting participant.
	 *
	 * @param Target Crafting participant to query.
	 * @return Known recipe templates, or empty array when target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Get All Known Recipes"))
	static TArray<UMounteaRecipeTemplate*> GetKnownRecipes(UObject* Target);

	/**
	 * Finds a known recipe by GUID on the crafting participant.
	 *
	 * @param Target Crafting participant to query.
	 * @param RecipeGuid Recipe identifier to search for.
	 * @return Matching recipe template, or nullptr if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="RecipeGuid"),
		meta=(DisplayName="Crafting Participant - Get Recipe"))
	static UMounteaRecipeTemplate* GetRecipe(UObject* Target, const FGuid& RecipeGuid);

	/**
	 * Returns recipes known by the participant for the specified crafting station type.
	 *
	 * @param Target Crafting participant to query.
	 * @param CraftingStationType Station type to filter by, or empty tag for all recipes.
	 * @return Matching recipe templates, or empty array when target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="CraftingStationType"),
		meta=(DisplayName="Crafting Participant - Get Recipes"))
	static TArray<UMounteaRecipeTemplate*> GetRecipes(UObject* Target, const FGameplayTag& CraftingStationType);

	/**
	 * Checks whether the participant knows the specified recipe.
	 *
	 * @param Target Crafting participant to query.
	 * @param RecipeTemplate Recipe to check.
	 * @return True if the recipe is known by the participant.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Is Recipe Known"))
	static bool IsRecipeKnown(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);

	/**
	 * Adds a recipe to the participant's known recipes.
	 *
	 * @param Target Crafting participant to update.
	 * @param RecipeTemplate Recipe to learn.
	 * @return True if the recipe was learned successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Learn Recipe"))
	static bool LearnRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);

	/**
	 * Removes a recipe from the participant's known recipes.
	 *
	 * @param Target Crafting participant to update.
	 * @param RecipeTemplate Recipe to forget.
	 * @return True if the recipe was forgotten successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Forget Recipe"))
	static bool ForgetRecipe(UObject* Target, UMounteaRecipeTemplate* RecipeTemplate);

	/**
	 * Checks whether the participant can craft the specified recipe.
	 *
	 * @param Target Crafting participant to query.
	 * @param TemplateToCraft Recipe to evaluate.
	 * @return True if crafting is possible.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Is Crafting Possible"))
	static bool IsCraftingPossible(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft);

	/**
	 * Calculates how many result item units can be crafted from the participant's available ingredients.
	 * If the recipe contains multiple ingredient options, the highest craftable option is returned.
	 *
	 * @param CraftingParticipant Crafting participant whose parent inventory provides the ingredients.
	 * @param TemplateToCraft Recipe to evaluate.
	 * @return Maximum number of result item units craftable from the available ingredients.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter, DefaultToSelf="CraftingParticipant"),
		meta=(Keywords="craft,craftable,amount,quantity,units,recipe,ingredients"),
		meta=(DisplayName="Crafting Participant - Get Maximum Craftable Units"))
	static int32 GetMaximumCraftableUnits(
		UPARAM(meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedCraftingParticipantInterface")) UObject* CraftingParticipant,
		UMounteaRecipeTemplate* TemplateToCraft);

	/**
	 * Starts crafting the specified recipe with the selected ingredient list.
	 *
	 * @param Target Crafting participant that performs crafting.
	 * @param TemplateToCraft Recipe to craft.
	 * @param Ingredients Ingredient option to consume.
	 * @return Crafting result describing whether the operation succeeded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Start Crafting"))
	static FMounteaCraftingResult StartCrafting(UObject* Target, UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);

	/**
	 * Returns the inventory used by the crafting participant.
	 *
	 * @param Target Crafting participant to query.
	 * @return Parent inventory interface, or empty interface when unavailable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Get Parent Inventory"))
	static TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory(UObject* Target);

	/**
	 * Sets the inventory used by the crafting participant.
	 *
	 * @param Target Crafting participant to update.
	 * @param NewParentInventory Inventory interface to use for crafting items and ingredients.
	 * @return True if the parent inventory was set successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Set Parent Inventory"))
	static bool SetParentInventory(UObject* Target, const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory);

	/**
	 * Starts using a crafting station with the specified participant.
	 *
	 * @param Target Crafting participant that will use the station.
	 * @param Station Crafting station to start using.
	 * @return True if the participant started using the station successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant - Start Using Crafting Station"))
	static bool StartUsingCraftingStation(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station);

	/**
	 * Stops the participant from using its current crafting station.
	 *
	 * @param Target Crafting participant to stop.
	 * @return True if station usage was stopped successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(ExpandBoolAsExecs="ReturnValue"),
		meta=(DisplayName="Crafting Participant - Stop Using Crafting Station"))
	static bool StopUsingCraftingStation(UObject* Target);
	
	/**
	 * Returns the crafting station currently used by the participant.
	 *
	 * @param Target Crafting participant to query.
	 * @return Current crafting station interface, or empty interface when unavailable.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Participant - Get Crafting Station"))
	static TScriptInterface<IMounteaAdvancedCraftingStationInterface> GetCraftingStation(UObject* Target);

public:

	/**
	 * Returns all recipe templates registered in the advanced crafting settings.
	 *
	 * @return Set of allowed recipe templates from crafting configuration.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Participant",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting - Get All Recipe Templates"))
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

	static int32 GetIngredientGroupCraftableCreations(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const UMounteaRecipeIngredientsList* IngredientGroup,
		TMap<UMounteaInventoryItemTemplate*, int32>& IngredientQuantityCache);

	static int32 GetRecipeCraftableCreations(
		const TScriptInterface<IMounteaAdvancedInventoryInterface>& Inventory,
		const UMounteaRecipeTemplate* Recipe,
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

	/**
	 * Returns the gameplay tag that identifies the crafting station type.
	 *
	 * @param Target Crafting station to query.
	 * @return Crafting place type tag, or empty tag when target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Get Crafting Place Type"))
	static FGameplayTag GetCraftingPlaceType(UObject* Target);

	/**
	 * Checks whether the crafting station is currently occupied.
	 *
	 * @param Target Crafting station to query.
	 * @return True if the crafting station is occupied.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Is Crafting Place Occupied"))
	static bool IsCraftingPlaceOccupied(UObject* Target);

	/**
	 * Returns how many participants can use the crafting station at once.
	 *
	 * @param Target Crafting station to query.
	 * @return Crafting station capacity, or INDEX_NONE when target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Get Crafting Place Capacity"))
	static int32 GetCraftingPlaceCapacity(UObject* Target);

	/**
	 * Checks whether the crafting station can currently be used.
	 *
	 * @param Target Crafting station to query.
	 * @return True if the station can be used.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaValidate),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Can Be Used"))
	static bool CanBeUsed(UObject* Target);

	/**
	 * Returns the current crafting station state.
	 *
	 * @param Target Crafting station to query.
	 * @return Current station state, or inactive when target is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaGetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Get Crafting Station State"))
	static ECraftingStationState GetCraftingStationState(UObject* Target);

	/**
	 * Updates the crafting station state.
	 *
	 * @param Target Crafting station to update.
	 * @param NewState New station state to apply.
	 * @return True if the station state was updated successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="NewState"),
		meta=(DisplayName="Crafting Station - Set Crafting Station State"))
	static bool SetCraftingStationState(UObject* Target, ECraftingStationState NewState);

	/**
	 * Starts using the crafting station with the specified participant.
	 *
	 * @param Target Crafting station to use.
	 * @param Participant Crafting participant that will use the station.
	 * @return True if the participant started using the station successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Start Using Station"))
	static bool StartUsing(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);

	/**
	 * Stops the specified participant from using the crafting station.
	 *
	 * @param Target Crafting station to stop using.
	 * @param Participant Crafting participant that should stop using the station.
	 * @return True if the participant stopped using the station successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mountea|Inventory & Equipment|Crafting|Station",
		meta=(MounteaSetter),
		meta=(DefaultToSelf="Target"),
		meta=(DisplayName="Crafting Station - Stop Using Station"))
	static bool StopUsing(UObject* Target, const TScriptInterface<IMounteaAdvancedCraftingParticipantInterface>& Participant);
	
/**
 * HELPERS
 */

public:
	
	/**
	 * Converts recipe templates to their recipe GUIDs.
	 * @param Recipes Recipes to read GUIDs from.
	 * @return GUIDs from valid recipe templates.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Helpers",
		meta=(MounteaGetter),
		meta=(DisplayName="Crafting - Get Recipe Guids"))
	static TArray<FGuid> GetRecipeGuids(const TArray<UMounteaRecipeTemplate*>& Recipes);

	/**
	 * Returns available recipes where the craftable item belongs to the specified inventory category.
	 *
	 * @param Target Crafting participant to query.
	 * @param CategoryTag Category to search by.
	 * @return Filtered list of available recipes by category.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Crafting|Helpers",
		meta=(MounteaGetter, MounteaInventoryCategoryPin="CategoryTag"),
		meta=(DefaultToSelf="Target"),
		meta=(AutoCreateRefTerm="CategoryTag"),
		meta=(DisplayName="Crafting Participant - Get Recipes By Category"))
	static TArray<UMounteaRecipeTemplate*> GetRecipesByCategory(
		UObject* Target,
		UPARAM(meta=(Categories="Mountea_Inventory.Categories,Mountea_Inventory.Category,Categories,Category")) const FGameplayTag& CategoryTag);
};
