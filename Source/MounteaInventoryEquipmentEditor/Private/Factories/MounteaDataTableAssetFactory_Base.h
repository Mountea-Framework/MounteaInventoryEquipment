// All rights reserved Dominik Morse 2022.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaDataTableAssetFactory_Base.generated.h"

class UMounteaInventoryItemsTable;

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaDataTableAssetFactory_Base : public UFactory
{
	GENERATED_BODY()

public:
	
	UMounteaDataTableAssetFactory_Base();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	UPROPERTY(BlueprintReadWrite, Category = "Data Table Factory")
	TObjectPtr<const class UScriptStruct> Struct;
};

UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaItemDescriptionsDataTableAssetFactory : public UMounteaDataTableAssetFactory_Base
{
	GENERATED_BODY()

public:
	
	UMounteaItemDescriptionsDataTableAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};