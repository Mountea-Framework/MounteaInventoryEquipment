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
#include "MounteaRecipeTemplate.generated.h"

class UMounteaRecipeItem;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, 
	meta=(DisplayName = "Inventory Recipe Template"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaRecipeTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
		Instanced,
		meta=(NoResetToDefault),
		//meta=(TitleProperty="item: {DisplayName} | slot: {EquipmentSlot}"),
		meta=(ShowInnerProperties, ShowOnlyInnerProperties),
		meta=(FullyExpand=true))
	TArray<TObjectPtr<UMounteaRecipeItem>> RecipeIngredientOptions;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
