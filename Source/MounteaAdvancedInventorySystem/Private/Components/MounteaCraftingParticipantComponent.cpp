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

#include "Net/UnrealNetwork.h"
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

void UMounteaCraftingParticipantComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UMounteaCraftingParticipantComponent, KnownRecipes, COND_InitialOrOwner);
}

void UMounteaCraftingParticipantComponent::OnRep_KnownRecipes()
{
	// KnownRecipes updated by replication; delegates fire via PostRecipeLearned_Client / PostRecipeForgotten_Client.
}

void UMounteaCraftingParticipantComponent::InitializeInventoryAndEquipment()
{
	auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
	if (!IsValid(inventoryComponent))
		LOG_ERROR(TEXT("[MounteaInventoryUIComponent] Cannot find 'Inventory' component in Parent! Loadouts will NOT work!"))
	else
	{
		Execute_SetParentInventory(this, inventoryComponent);
		ensureMsgf(RelatedInventory.GetObject() != nullptr, TEXT("[MounteaAdvancedInventoryLoadoutComponent] Failed to update 'RelatedInventory'"));
	}
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::GetKnownRecipes_Implementation() const
{
	return ResolveKnownRecipeTemplates(KnownRecipes);
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::GetRecipes_Implementation(const FGameplayTag& CraftingStationType) const
{
	const TArray<UMounteaRecipeTemplate*> knownRecipes = ResolveKnownRecipeTemplates(KnownRecipes);
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

	const TArray<UMounteaRecipeTemplate*> knownRecipes = ResolveKnownRecipeTemplates(KnownRecipes);
	UMounteaRecipeTemplate* const* foundRecipe = knownRecipes.FindByPredicate(
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

	if (!GetOwner()->HasAuthority())
	{
		Server_LearnRecipe(RecipeTemplate);
		return true;
	}

	KnownRecipes.Add(RecipeTemplate->RecipeGuid);
	PostRecipeLearned_Client(RecipeTemplate);
	OnRecipeLearned.Broadcast(RecipeTemplate);
	return true;
}

bool UMounteaCraftingParticipantComponent::ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	if (!Execute_IsRecipeKnown(this, RecipeTemplate))
		return false;

	if (!GetOwner()->HasAuthority())
	{
		Server_ForgetRecipe(RecipeTemplate);
		return true;
	}

	KnownRecipes.Remove(RecipeTemplate->RecipeGuid);
	PostRecipeForgotten_Client(RecipeTemplate);
	OnRecipeForgotten.Broadcast(RecipeTemplate);
	return true;
}

bool UMounteaCraftingParticipantComponent::IsCraftingPossible_Implementation(UMounteaRecipeTemplate* TemplateToCraft) const
{
	return Execute_IsRecipeKnown(this, TemplateToCraft);
}

FMounteaCraftingResult UMounteaCraftingParticipantComponent::StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_StartCrafting(TemplateToCraft, Ingredients);
		return FMounteaCraftingResult{};
	}

	FMounteaCraftingResult result = UMounteaCraftingStatics::CraftItem(this, TemplateToCraft, Ingredients);
	PostCraftingFinished_Client(result);
	OnCraftingFinished.Broadcast(result);
	return result;
}

bool UMounteaCraftingParticipantComponent::SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	if (RelatedInventory == NewParentInventory)
		return false;
	RelatedInventory = NewParentInventory;
	return true;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::ResolveKnownRecipeTemplates(const TArray<FGuid>& KnownRecipeGuids)
{
	if (KnownRecipeGuids.Num() == 0)
		return TArray<UMounteaRecipeTemplate*>();

	const TArray<UMounteaRecipeTemplate*> allRecipes = UMounteaCraftingStatics::GetAllRecipeTemplates().Array();
	if (allRecipes.Num() == 0)
		return TArray<UMounteaRecipeTemplate*>();

	TArray<UMounteaRecipeTemplate*> result;
	result.Reserve(KnownRecipeGuids.Num());

	const auto allRecipesArray = allRecipes;
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

void UMounteaCraftingParticipantComponent::Server_LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	Execute_LearnRecipe(this, RecipeTemplate);
}

void UMounteaCraftingParticipantComponent::Server_ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	Execute_ForgetRecipe(this, RecipeTemplate);
}

void UMounteaCraftingParticipantComponent::Server_StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients)
{
	Execute_StartCrafting(this, TemplateToCraft, Ingredients);
}

void UMounteaCraftingParticipantComponent::PostRecipeLearned_Client_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	OnRecipeLearned.Broadcast(RecipeTemplate);
}

void UMounteaCraftingParticipantComponent::PostRecipeForgotten_Client_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	OnRecipeForgotten.Broadcast(RecipeTemplate);
}

void UMounteaCraftingParticipantComponent::PostCraftingFinished_Client_Implementation(const FMounteaCraftingResult& Result)
{
	OnCraftingFinished.Broadcast(Result);
}

