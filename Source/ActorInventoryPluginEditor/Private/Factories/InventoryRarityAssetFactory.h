// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InventoryRarityAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGINEDITOR_API UInventoryRarityAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:

	UInventoryRarityAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
