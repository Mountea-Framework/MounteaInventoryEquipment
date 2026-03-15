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
#include "MounteaRecipeIngredient.generated.h"

class UMounteaInventoryItemTemplate;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, DefaultToInstanced, EditInlineNew,
	AutoExpandCategories=("Mountea"),
	HideCategories=("Cooking","Collision","Private"),
	meta=(DisplayName="Mountea Inventory Recipe Item Ingredient"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaRecipeIngredient : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	TObjectPtr<UMounteaInventoryItemTemplate> IngredientSource = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		meta=(NoResetToDefault))
	int32 RequiredQuantity = 1;
};
