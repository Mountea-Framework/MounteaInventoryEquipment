// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaInventoryItemAdditionalDataFactory.generated.h"

class UMounteaItemAdditionalData;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaInventoryItemAdditionalDataFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaInventoryItemAdditionalDataFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaItemAdditionalData> ParentClass;
};