// All rights reserved Dominik Morse 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaThemeAssetFactory.generated.h"

class UMounteaInventoryThemeConfig;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaThemeAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaThemeAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryThemeConfig> ParentClass;
};