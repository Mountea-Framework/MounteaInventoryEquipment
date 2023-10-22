// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaContentThemeAssetFactory.generated.h"

class UContentTheme;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaContentThemeAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaContentThemeAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

protected:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UContentTheme> ParentClass;
};

UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UTextThemeAssetFactory : public UMounteaContentThemeAssetFactory
{
	GENERATED_BODY()

public:
	
	UTextThemeAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;
};

UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UCategoryThemeAssetFactory : public UMounteaContentThemeAssetFactory
{
	GENERATED_BODY()

public:
	
	UCategoryThemeAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;
};

UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API USlotThemeAssetFactory : public UMounteaContentThemeAssetFactory
{
	GENERATED_BODY()

public:
	
	USlotThemeAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;
};