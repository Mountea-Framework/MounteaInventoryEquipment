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


#include "Components/MounteaCraftingParticipantComponent.h"

#include "Definitions/MounteaCraftingBaseDataTypes.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Statics/MounteaCraftingStatics.h"

UMounteaCraftingParticipantComponent::UMounteaCraftingParticipantComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaCraftingParticipantComponent::SetComponentTickEnabled(false);
	
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Crafting") } );
}

void UMounteaCraftingParticipantComponent::BeginPlay()
{
	Super::BeginPlay();	
	
	InitializeInventoryAndEquipment();
}

void UMounteaCraftingParticipantComponent::InitializeInventoryAndEquipment()
{
	auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
	if (!IsValid(inventoryComponent))
		LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory' component in Parent! Loadouts will NOT work!"))
	else
	{
		RelatedInventory = inventoryComponent;
		ensureMsgf(RelatedInventory.GetObject() != nullptr, TEXT("[MounteaAdvancedInventoryLoadoutComponent] Failed to update 'RelatedInventory'"));
	}
}

TSet<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::GetKnownRecipes_Implementation() const
{
	return ResolveKnownRecipeTemplates(KnownRecipes);
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::GetRecipes_Implementation(const FGameplayTag& CraftingStationType) const
{
	const TSet<UMounteaRecipeTemplate*> knownRecipes = ResolveKnownRecipeTemplates(KnownRecipes);
	if (knownRecipes.Num() == 0)
		return TArray<UMounteaRecipeTemplate*>();

	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(knownRecipes.Num());

	for (UMounteaRecipeTemplate* recipe : knownRecipes)
	{
		if (!IsValid(recipe))
		continue;

		if (!CraftingStationType.IsValid() || recipe->RequiredCraftingPlace == CraftingStationType)
			result.Add(recipe);
	}

	return result;
}

UMounteaRecipeTemplate* UMounteaCraftingParticipantComponent::GetRecipe_Implementation(const FGuid& RecipeGuid) const
{
	if (!KnownRecipes.Contains(RecipeGuid))
		return nullptr;

	const TSet<UMounteaRecipeTemplate*> knownRecipes = ResolveKnownRecipeTemplates(KnownRecipes);
	UMounteaRecipeTemplate* const* foundRecipe = knownRecipes.Array().FindByPredicate(
		[&RecipeGuid](const UMounteaRecipeTemplate* recipe)
		{
			return IsValid(recipe) && recipe->RecipeGuid == RecipeGuid;
		});

	return foundRecipe != nullptr ? *foundRecipe : nullptr;
}

bool UMounteaCraftingParticipantComponent::IsRecipeKnown_Implementation(UMounteaRecipeTemplate* RecipeTemplate) const
{
	return KnownRecipes.Contains(RecipeTemplate->RecipeGuid);
}

bool UMounteaCraftingParticipantComponent::LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	if (Execute_IsRecipeKnown(this, RecipeTemplate))
		return false;
	KnownRecipes.Add(RecipeTemplate->RecipeGuid);
	
	OnRecipeLearned.Broadcast(RecipeTemplate);
	return true;
}

bool UMounteaCraftingParticipantComponent::ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	if (!Execute_IsRecipeKnown(this, RecipeTemplate))
		return false;
	OnRecipeForgotten.Broadcast(RecipeTemplate);
	
	KnownRecipes.Remove(RecipeTemplate->RecipeGuid);
	return true;
}

bool UMounteaCraftingParticipantComponent::IsCraftingPossible_Implementation(UMounteaRecipeTemplate* TemplateToCraft) const
{
	return Execute_IsRecipeKnown(this, TemplateToCraft);
}

FMounteaCraftingResult UMounteaCraftingParticipantComponent::StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	FMounteaCraftingResult result = UMounteaCraftingStatics::CraftItem(this, TemplateToCraft, Ingredients);
	return result;
}

TSet<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::ResolveKnownRecipeTemplates(const TSet<FGuid>& KnownRecipeGuids)
{
	if (KnownRecipeGuids.Num() == 0)
		return TSet<UMounteaRecipeTemplate*>();

	const TSet<UMounteaRecipeTemplate*> allRecipes = UMounteaCraftingStatics::GetAllRecipeTemplates();
	if (allRecipes.Num() == 0)
		return TSet<UMounteaRecipeTemplate*>();

	TSet<UMounteaRecipeTemplate*> result;
	result.Reserve(KnownRecipeGuids.Num());

	const auto allRecipesArray = allRecipes.Array();
	for (const FGuid& knownRecipeGuid : KnownRecipeGuids)
	{
		UMounteaRecipeTemplate* const* foundRecipe = allRecipesArray.FindByPredicate(
			[&knownRecipeGuid](const UMounteaRecipeTemplate* recipe)
			{
				return IsValid(recipe) && recipe->RecipeGuid == knownRecipeGuid;
			});

		if (foundRecipe != nullptr && IsValid(*foundRecipe))
			result.Add(*foundRecipe);
	}

	return result;
}

