// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InventoryCategoryAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGINEDITOR_API UInventoryCategoryAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:

	UInventoryCategoryAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
