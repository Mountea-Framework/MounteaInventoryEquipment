// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaItemsDataTableAssetFactory.generated.h"

class UMounteaInventoryItemsTable;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaItemsDataTableAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaItemsDataTableAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	UPROPERTY(BlueprintReadWrite, Category = "Data Table Factory")
	TObjectPtr<const class UScriptStruct> Struct;
};