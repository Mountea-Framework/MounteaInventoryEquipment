// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InventoryKeyActionAssetFactory.generated.h"

class UInventoryKeyAction;

/**
 * 
 */
UCLASS()
class ACTORINVENTORYPLUGINEDITOR_API UInventoryKeyActionAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:

	UInventoryKeyActionAssetFactory(const FObjectInitializer& ObjectInitializer);
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UInventoryKeyAction> ParentClass;
};


