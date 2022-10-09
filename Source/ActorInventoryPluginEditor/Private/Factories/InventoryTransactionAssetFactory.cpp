// All rights reserved Dominik Pavlicek 2022.


#include "InventoryTransactionAssetFactory.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "Helpers/InventoryHelpers.h"

UInventoryTransactionAssetActions::UInventoryTransactionAssetActions()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UInventoryTransaction::StaticClass();

	ParentClass = SupportedClass;
}

UObject* UInventoryTransactionAssetActions::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Something is not right!
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ParentClass ? FText::FromString(ParentClass->GetName()) : NSLOCTEXT("UnrealEd", "Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), Args));
		return nullptr;
	}

	// Create new Blueprint
	return FKismetEditorUtilities::CreateBlueprint(
		ParentClass,
		InParent,
		Name,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		NAME_None
	);
}
