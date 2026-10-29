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
#include "Definitions/MounteaCraftingBaseEnums.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingStationInterface.h"
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
	DOREPLIFETIME_CONDITION(UMounteaCraftingParticipantComponent, CraftingStation, COND_InitialOrOwner);
}

void UMounteaCraftingParticipantComponent::OnRep_KnownRecipes()
{
}

void UMounteaCraftingParticipantComponent::OnRep_CraftingStation()
{
}

void UMounteaCraftingParticipantComponent::InitializeInventoryAndEquipment()
{
	auto inventoryComponent = GetOwner()->FindComponentByInterface(UMounteaAdvancedInventoryInterface::StaticClass());
	if (!IsValid(inventoryComponent))
		LOG_ERROR(TEXT("[MounteaInventoryComponent] Cannot find 'Inventory' component in Parent! Loadouts will NOT work!"))
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
	return IsValid(RecipeTemplate) && KnownRecipes.Contains(RecipeTemplate->RecipeGuid);
}

bool UMounteaCraftingParticipantComponent::LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	if (!IsValid(RecipeTemplate))
		return false;
	
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
	if (!IsValid(TemplateToCraft) || TemplateToCraft->QuantityPerCreation <= 0
		|| !IsValid(TemplateToCraft->ResultItem.LoadSynchronous())
		|| !Execute_IsRecipeKnown(this, TemplateToCraft) || !RelatedInventory)
		return false;

	if (TemplateToCraft->RequiredCraftingPlace.IsValid()
		&& (!UMounteaCraftingStatics::IsValidCraftingPlace(CraftingStation.GetObject())
			|| TemplateToCraft->RequiredCraftingPlace != UMounteaCraftingStatics::GetCraftingPlaceType(CraftingStation.GetObject())))
		return false;

	TMap<UMounteaInventoryItemTemplate*, int32> ingredientQuantityCache;
	return UMounteaCraftingStatics::IsRecipeSatisfiedByIngredients(RelatedInventory, TemplateToCraft, ingredientQuantityCache);
}

FMounteaCraftingResult UMounteaCraftingParticipantComponent::StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_StartCrafting(TemplateToCraft);
		return FMounteaCraftingResult{};
	}

	OnCraftingStarted.Broadcast(CraftingStation);
	PostCraftingStarted_Client(CraftingStation);
	FMounteaCraftingResult result = UMounteaCraftingStatics::CraftItem(this, TemplateToCraft);
	
	// TODO: Add possible delay IF crafting recipe takes time
	
	PostCraftingFinished_Client(result);
	OnCraftingFinished.Broadcast(result);
	return result;
}

void UMounteaCraftingParticipantComponent::StopCrafting_Implementation()
{
	if (!GetOwner()->HasAuthority())
	{
		Server_StopCrafting();
		return;
	}
	
	PostCraftingStopped_Client(CraftingStation);
	OnCraftingStopped.Broadcast(CraftingStation);
	
	if (CraftingStation.GetObject())
		Execute_StopUsingCraftingStation(this);
	
	CraftingStation = nullptr;
	return;
}

bool UMounteaCraftingParticipantComponent::SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory)
{
	if (RelatedInventory == NewParentInventory)
		return false;
	RelatedInventory = NewParentInventory;
	return true;
}

bool UMounteaCraftingParticipantComponent::StartUsingCraftingStation_Implementation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station)
{
	if (!IsValid(Station.GetObject()))
		return false;
	if (Station == CraftingStation)
		return false;

	if (!UMounteaCraftingStatics::DoesHaveAnyRecipes(this))
		return false;
	
	if (!GetOwner()->HasAuthority())
	{
		Server_StartUsingCraftingStation(Station);
		return true;
	}

	CraftingStation = Station;
	IMounteaAdvancedCraftingStationInterface::Execute_StartUsing(Station.GetObject(), this);
	OnCraftingStarted.Broadcast(CraftingStation);
	PostCraftingStarted_Client(CraftingStation);
	return true;
}

bool UMounteaCraftingParticipantComponent::StopUsingCraftingStation_Implementation()
{
	if (!IsValid(CraftingStation.GetObject()))
		return false;

	if (!GetOwner()->HasAuthority())
	{
		Server_StopUsingCraftingStation();
		return true;
	}

	PostCraftingStopped_Client(CraftingStation);
	OnCraftingStopped.Broadcast(CraftingStation);
	IMounteaAdvancedCraftingStationInterface::Execute_StopUsing(CraftingStation.GetObject(), this);
	CraftingStation = nullptr;
	return true;
}

TArray<UMounteaRecipeTemplate*> UMounteaCraftingParticipantComponent::ResolveKnownRecipeTemplates(const TArray<FGuid>& KnownRecipeGuids)
{
	if (KnownRecipeGuids.Num() == 0)
		return TArray<UMounteaRecipeTemplate*>();

	const TArray<UMounteaRecipeTemplate*> allRecipes = UMounteaCraftingStatics::GetAllRecipeTemplates();
	if (allRecipes.Num() == 0)
		return TArray<UMounteaRecipeTemplate*>();

	return allRecipes;
}

void UMounteaCraftingParticipantComponent::Server_StopCrafting_Implementation()
{
	Execute_StopCrafting(this);
}

void UMounteaCraftingParticipantComponent::PostCraftingStopped_Client_Implementation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& OptionalCraftingStation)
{
	OnCraftingStopped.Broadcast(OptionalCraftingStation);
}

void UMounteaCraftingParticipantComponent::PostCraftingStarted_Client_Implementation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& OptionalCraftingStation)
{
	OnCraftingStarted.Broadcast(OptionalCraftingStation);
}

void UMounteaCraftingParticipantComponent::Server_LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	Execute_LearnRecipe(this, RecipeTemplate);
}

void UMounteaCraftingParticipantComponent::Server_ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate)
{
	Execute_ForgetRecipe(this, RecipeTemplate);
}

void UMounteaCraftingParticipantComponent::Server_StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft)
{
	Execute_StartCrafting(this, TemplateToCraft);
}

void UMounteaCraftingParticipantComponent::Server_StartUsingCraftingStation_Implementation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station)
{
	Execute_StartUsingCraftingStation(this, Station);
}

void UMounteaCraftingParticipantComponent::Server_StopUsingCraftingStation_Implementation()
{
	Execute_StopUsingCraftingStation(this);
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
