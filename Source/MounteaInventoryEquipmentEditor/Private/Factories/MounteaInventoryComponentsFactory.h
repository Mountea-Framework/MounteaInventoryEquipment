// All rights reserved Dominik Pavlicek 2023

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaInventoryComponentsFactory.generated.h"

class UMounteaInventoryComponent;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaInventoryComponentFactory : public UFactory
{
	GENERATED_BODY()

public:

	UMounteaInventoryComponentFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryComponent> ParentClass;
};
