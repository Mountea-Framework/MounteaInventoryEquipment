// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaAdvancedInventoryItemTemplate_Factory.generated.h"

class UMounteaInventoryItemTemplate;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaAdvancedInventoryItemTemplate_Factory : public UFactory
{
	GENERATED_BODY()
	
public:
	UMounteaAdvancedInventoryItemTemplate_Factory();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, 
		EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, 
		FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

private:
	static UMounteaInventoryItemTemplate* CreateSingleTemplate(const UClass* InClass, UObject* InParent, 
	                                                           FName InName, EObjectFlags Flags, const FString& JsonString, FFeedbackContext* Warn);

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryItemTemplate> ParentClass;
};
