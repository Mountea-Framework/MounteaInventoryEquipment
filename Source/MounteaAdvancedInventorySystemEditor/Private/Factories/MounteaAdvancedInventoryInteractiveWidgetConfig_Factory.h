// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaAdvancedInventoryInteractiveWidgetConfig_Factory.generated.h"

class UMounteaAdvancedInventoryInteractiveWidgetConfig;

UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventoryInteractiveWidgetConfig_Factory : public UFactory
{
	GENERATED_BODY()
public:

	UMounteaAdvancedInventoryInteractiveWidgetConfig_Factory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaAdvancedInventoryInteractiveWidgetConfig> ParentClass;
};
