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
#include "MounteaRecipeIngredient_Factory.generated.h"

class UMounteaRecipeIngredient;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaRecipeIngredient_Factory : public UFactory
{
	GENERATED_BODY()
public:

	UMounteaRecipeIngredient_Factory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context,
		FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	UPROPERTY()
	TSubclassOf<UMounteaRecipeIngredient> ParentClass;
};

