// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaInventoryItemBlueprintFactory.generated.h"

class UMounteaInventoryItemBlueprint;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaInventoryItemBlueprintFactory : public UFactory
{
	GENERATED_BODY()

public:

	UMounteaInventoryItemBlueprintFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryItemBlueprint> ParentClass;

};
