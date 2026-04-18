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
#include "Factories/Factory.h"
#include "MounteaRecipeIngredientsList_Factory.generated.h"

class UMounteaRecipeIngredientsList;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaRecipeIngredientsList_Factory : public UFactory
{
	GENERATED_BODY()
public:

	UMounteaRecipeIngredientsList_Factory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context,
		FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	UPROPERTY()
	TSubclassOf<UMounteaRecipeIngredientsList> ParentClass;
};

