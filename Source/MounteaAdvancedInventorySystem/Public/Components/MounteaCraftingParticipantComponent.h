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
#include "Components/ActorComponent.h"
#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantInterface.h"
#include "MounteaCraftingParticipantComponent.generated.h"

class IMounteaAdvancedCraftingStationInterface;
struct FMounteaCraftingResult;
class IMounteaAdvancedInventoryInterface;

/**
 * UMounteaCraftingParticipantComponent represents an actor capable of interacting with crafting stations.
 * It manages known recipes, crafting actions, and the currently-used station reference,
 * replicated with server authority and client notification RPCs.
 *
 * @see IMounteaAdvancedCraftingParticipantInterface
 * @see UMounteaCraftingStationComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Crafting","Mountea|Crafting"),
	AutoCollapseCategories=("Variable,Sockets,Tags,Component Tick,Component Replication,Activation,Events,Replication,Asset User Data,Navigation"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Crafting Participant Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingParticipantComponent : public UActorComponent, public IMounteaAdvancedCraftingParticipantInterface
{
	GENERATED_BODY()

public:
	UMounteaCraftingParticipantComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void InitializeInventoryAndEquipment();

	UFUNCTION()
	void OnRep_KnownRecipes();

	UFUNCTION()
	void OnRep_CraftingStation();

public:

	virtual TArray<UMounteaRecipeTemplate*> GetKnownRecipes_Implementation() const override;
	virtual TArray<UMounteaRecipeTemplate*> GetRecipes_Implementation(const FGameplayTag& CraftingStationType) const override;
	virtual UMounteaRecipeTemplate* GetRecipe_Implementation(const FGuid& RecipeGuid) const override;
	virtual bool IsRecipeKnown_Implementation(UMounteaRecipeTemplate* RecipeTemplate) const override;
	virtual bool LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate) override;
	virtual bool ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate) override;
	virtual bool IsCraftingPossible_Implementation(UMounteaRecipeTemplate* TemplateToCraft) const override;
	virtual FMounteaCraftingResult StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients) override;
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const override
	{
		return RelatedInventory;
	}
	virtual bool SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) override;

	virtual FOnCraftingFinished& GetOnCraftingFinishedEventHandle() override
	{
		return OnCraftingFinished;
	}
	virtual FOnRecipeLearned& GetOnRecipeLearnedEventHandle() override
	{
		return OnRecipeLearned;
	}
	virtual FOnRecipeForgotten& GetOnRecipeForgottenEventHandle() override
	{
		return OnRecipeForgotten;
	}
	virtual bool StartUsingCraftingStation_Implementation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station) override;
	virtual bool StopUsingCraftingStation_Implementation() override;

protected:
	static TArray<UMounteaRecipeTemplate*> ResolveKnownRecipeTemplates(const TArray<FGuid>& KnownRecipeGuids);

public:

	UPROPERTY(BlueprintAssignable, Category="Crafting")
	FOnCraftingFinished OnCraftingFinished;

	UPROPERTY(BlueprintAssignable, Category="Crafting")
	FOnRecipeLearned OnRecipeLearned;

	UPROPERTY(BlueprintAssignable, Category="Crafting")
	FOnRecipeForgotten OnRecipeForgotten;

	UPROPERTY(SaveGame, ReplicatedUsing=OnRep_KnownRecipes, VisibleDefaultsOnly, BlueprintReadOnly, Category="Crafting",
		meta=(NoResetToDefault))
	TArray<FGuid> KnownRecipes;

	/**
	 * Cached inventory interface found on the owning actor.
	 *
	 * This is initialized during BeginPlay and used as the target inventory when loading
	 * items from the configured loadout.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoResetToDefault),
		meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedInventoryInterface> RelatedInventory;

	UPROPERTY(ReplicatedUsing=OnRep_CraftingStation, VisibleAnywhere, BlueprintReadOnly, Category="Configuration",
		meta=(NoResetToDefault),
		meta=(DisplayThumbnail=false))
	TScriptInterface<IMounteaAdvancedCraftingStationInterface> CraftingStation;

protected:

	UFUNCTION(Server, Reliable)
	void Server_LearnRecipe(UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(Server, Reliable)
	void Server_ForgetRecipe(UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(Server, Reliable)
	void Server_StartCrafting(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients);

	UFUNCTION(Server, Reliable)
	void Server_StartUsingCraftingStation(const TScriptInterface<IMounteaAdvancedCraftingStationInterface>& Station);

	UFUNCTION(Server, Reliable)
	void Server_StopUsingCraftingStation();

	UFUNCTION(Client, Unreliable)
	void PostRecipeLearned_Client(UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(Client, Unreliable)
	void PostRecipeForgotten_Client(UMounteaRecipeTemplate* RecipeTemplate);

	UFUNCTION(Client, Unreliable)
	void PostCraftingFinished_Client(const FMounteaCraftingResult& Result);
};
