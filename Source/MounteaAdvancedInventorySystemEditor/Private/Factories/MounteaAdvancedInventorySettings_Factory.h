// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaAdvancedInventorySettings_Factory.generated.h"

class UMounteaAdvancedInventorySettingsConfig;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventorySettings_Factory : public UFactory
{
	GENERATED_BODY()
public:

	UMounteaAdvancedInventorySettings_Factory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaAdvancedInventorySettingsConfig> ParentClass;
};
