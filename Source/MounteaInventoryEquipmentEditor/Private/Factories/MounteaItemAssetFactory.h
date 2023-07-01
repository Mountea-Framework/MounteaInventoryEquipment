// All rights reserved Dominik Pavlicek 2023

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MounteaItemAssetFactory.generated.h"

class UMounteaInventoryItemsTable;
class UMounteaInventoryItemBase;
/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaItemAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	UMounteaItemAssetFactory();
	
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ConfigureProperties() override;

	void SetParentClass(const TSubclassOf<UMounteaInventoryItemsTable>& InParentClass);
	void SetSource(UMounteaInventoryItemsTable* InSourceTable, const FName& InSourceRow);

private:
	// Holds the template of the class we are building
	UPROPERTY()
	TSubclassOf<UMounteaInventoryItemBase> ParentClass;

	UPROPERTY()
	UMounteaInventoryItemsTable* SourceTable;

	UPROPERTY()
	FName SourceRow;
};
