// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaAdvancedEquipmentComponent_Factory.generated.h"

class UMounteaEquipmentComponent;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedEquipmentComponent_Factory : public UFactory
{
	GENERATED_BODY()

public:
	UMounteaAdvancedEquipmentComponent_Factory();

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaEquipmentComponent> ParentClass;
};
