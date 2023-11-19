// All rights reserved Dominik Morse 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaCategoryAssetFactory.generated.h"


class UMounteaInventoryItemCategory;
/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaCategoryAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaCategoryAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryItemCategory> ParentClass;
};