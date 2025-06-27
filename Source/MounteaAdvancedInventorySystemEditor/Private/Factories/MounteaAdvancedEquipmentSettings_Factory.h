// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaAdvancedEquipmentSettings_Factory.generated.h"

class UMounteaAdvancedEquipmentSettingsConfig;
/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedEquipmentSettings_Factory : public UFactory
{
	GENERATED_BODY()
	
	UMounteaAdvancedEquipmentSettings_Factory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaAdvancedEquipmentSettingsConfig> ParentClass;
};
