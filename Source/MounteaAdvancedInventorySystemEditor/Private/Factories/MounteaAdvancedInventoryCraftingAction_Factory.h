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
#include "MounteaAdvancedInventoryCraftingAction_Factory.generated.h"

class UMounteaCraftingAction;

/**
 * Creates Blueprint assets derived from a chosen UMounteaCraftingAction child class.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventoryCraftingAction_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventoryCraftingAction_Factory();

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:

	// Parent class chosen in ConfigureProperties
	UPROPERTY()
	TSubclassOf<UMounteaCraftingAction> ParentClass;
};
