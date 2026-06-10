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

#include "MounteaAdvancedInventoryModalsDataTable_Factory.h"

#include "Definitions/MounteaAdvancedInventoryGlobalUIDataTypes.h"
#include "Definitions/MounteaAdvancedInventoryModalsDataTable.h"
#include "Engine/DataTable.h"

#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryModalsDataTable_Factory"

UMounteaAdvancedInventoryModalsDataTable_Factory::UMounteaAdvancedInventoryModalsDataTable_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaAdvancedInventoryModalsDataTable::StaticClass();
}

UObject* UMounteaAdvancedInventoryModalsDataTable_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UDataTable* newDataTable = NewObject<UDataTable>(InParent, UDataTable::StaticClass(), Name, Flags, Context);
	newDataTable->RowStruct = FMounteaModalsConfig::StaticStruct();

	return newDataTable;
}

FText UMounteaAdvancedInventoryModalsDataTable_Factory::GetDisplayName() const
{
	return LOCTEXT("MounteaAdvancedInventoryModalsDataTable_Factory_DisplayName", "Mountea Advanced Inventory Modals Data Table");
}

#undef LOCTEXT_NAMESPACE
