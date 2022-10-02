// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InventoryTransactionAssetFactory.generated.h"

class UInventoryTransaction;

/**
 *  
 */
UCLASS()
class ACTORINVENTORYPLUGINEDITOR_API UInventoryTransactionAssetActions : public UFactory
{
	GENERATED_BODY()

public:
	
	UInventoryTransactionAssetActions();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:

	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UInventoryTransaction> ParentClass;
	
};
