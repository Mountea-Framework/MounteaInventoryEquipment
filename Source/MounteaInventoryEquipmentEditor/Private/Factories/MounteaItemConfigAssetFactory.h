// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaItemConfigAssetFactory.generated.h"

class UMounteaInventoryItemConfig;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaItemConfigAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaItemConfigAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryItemConfig> ParentClass;
};