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
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MounteaRecipeTemplate.generated.h"

class UMounteaRecipeIngredientsList;
class UMounteaInventoryItemTemplate;
class UMounteaInventoryItemTemplate_Recipe;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, 
	meta=(DisplayName = "Inventory Recipe Entry"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaRecipeTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	/**
	 * OPTIONAL
	 * 
	 * If crafting requires a specific place, specify the tag which is required.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault),
		meta=(Categories="Mountea_Inventory.Crafting,Crafting"))
	FGameplayTag RequiredCraftingPlace;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	TSoftObjectPtr<UMounteaInventoryItemTemplate> ResultItem = nullptr;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	int32 QuantityPerCreation = 1;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault),
		meta=(UIMin=0.f, ClampMin=0.f),
		meta=(Units="seconds"))
	float CraftingTime = 0.f;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		Instanced,
		meta=(NoResetToDefault),
		meta=(ShowInnerProperties, ShowOnlyInnerProperties),
		meta=(FullyExpand=true))
	TArray<TObjectPtr<UMounteaRecipeIngredientsList>> RecipeIngredientOptions;
	
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	FGuid RecipeGuid = FGuid::NewGuid();

	/**
	 * Holds reference of Item (Recipe), which granted this Recipe to known recipes.
	 * Some Items grant only temporary knowledge, and if the related item is NOT in the inventory (grimoire, for example),
	 * then we must not allow this recipe to be visible.
	 */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	TSoftObjectPtr<UMounteaInventoryItemTemplate_Recipe> RecipeSource;

protected:

#if WITH_EDITORONLY_DATA
	// Absolute path to the file this recipe was imported from (set automatically by the import pipeline).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Technical Data", AdvancedDisplay,
		meta=(NoResetToDefault))
	FString SourceFilePath;
#endif

public:

#if WITH_EDITOR
	FString GetSourceFilePath() const;
	void SetSourceFilePath(const FString& InPath);

	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
