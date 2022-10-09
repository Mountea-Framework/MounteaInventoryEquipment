// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InventoryTypeDefinitionAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGINEDITOR_API UInventoryTypeDefinitionAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UInventoryTypeDefinitionAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
