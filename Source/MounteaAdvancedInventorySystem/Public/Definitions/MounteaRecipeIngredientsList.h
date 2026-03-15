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
#include "Engine/DataAsset.h"
#include "MounteaRecipeIngredientsList.generated.h"

class UMounteaRecipeIngredient;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea"),
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Inventory Recipe Ingredients"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaRecipeIngredientsList : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		Instanced,
		meta=(NoResetToDefault),
		//meta=(TitleProperty="item: {DisplayName} | slot: {EquipmentSlot}"),
		meta=(ShowInnerProperties, ShowOnlyInnerProperties),
		meta=(FullyExpand=true))
	TArray<TObjectPtr<UMounteaRecipeIngredient>> RecipeIngredients;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
