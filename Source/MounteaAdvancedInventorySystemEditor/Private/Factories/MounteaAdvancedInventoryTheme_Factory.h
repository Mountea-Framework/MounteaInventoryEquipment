// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaAdvancedInventoryTheme_Factory.generated.h"

class UMounteaAdvancedInventoryThemeConfig;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventoryTheme_Factory : public UFactory
{
	GENERATED_BODY()
public:

	UMounteaAdvancedInventoryTheme_Factory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaAdvancedInventoryThemeConfig> ParentClass;
};
