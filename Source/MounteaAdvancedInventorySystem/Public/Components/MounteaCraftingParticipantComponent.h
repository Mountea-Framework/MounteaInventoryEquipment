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

struct FMounteaCraftingResult;

UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Crafting","Mountea|Crafting"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Crafting Participant Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaCraftingParticipantComponent : public UActorComponent, public IMounteaAdvancedCraftingParticipantInterface
{
	GENERATED_BODY()

public:
	UMounteaCraftingParticipantComponent();

protected:
	virtual void BeginPlay() override;

public:
	
	virtual TSet<UMounteaRecipeTemplate*> GetKnownRecipes_Implementation() const override;
	virtual TArray<UMounteaRecipeTemplate*> GetRecipes_Implementation(const FGameplayTag& CraftingStationType) const override;
	virtual UMounteaRecipeTemplate* GetRecipe_Implementation(const FGuid& RecipeGuid) const override;
	virtual bool IsRecipeKnown_Implementation(UMounteaRecipeTemplate* RecipeTemplate) const override;
	virtual bool LearnRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate) override;
	virtual bool ForgetRecipe_Implementation(UMounteaRecipeTemplate* RecipeTemplate) override;
	virtual bool IsCraftingPossible_Implementation(UMounteaRecipeTemplate* TemplateToCraft) const override;
	virtual FMounteaCraftingResult StartCrafting_Implementation(UMounteaRecipeTemplate* TemplateToCraft, UMounteaRecipeIngredientsList* Ingredients) override;
	
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
	
protected:
	static TSet<UMounteaRecipeTemplate*> ResolveKnownRecipeTemplates(const TSet<FGuid>& KnownRecipeGuids);
	
public:
	
	UPROPERTY(BlueprintAssignable, Category="Crafting")
	FOnCraftingFinished OnCraftingFinished;
	
	UPROPERTY(BlueprintAssignable, Category="Crafting")
	FOnRecipeLearned OnRecipeLearned;
	
	UPROPERTY(BlueprintAssignable, Category="Crafting")
	FOnRecipeForgotten OnRecipeForgotten;
	
	UPROPERTY(SaveGame, VisibleDefaultsOnly, BlueprintReadOnly, Category="Crafting",
		meta=(NoResetToDefault))
	TSet<FGuid> KnownRecipes;
};
