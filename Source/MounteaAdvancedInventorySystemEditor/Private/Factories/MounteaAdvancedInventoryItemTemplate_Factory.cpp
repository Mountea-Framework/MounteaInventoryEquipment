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


#include "MounteaAdvancedInventoryItemTemplate_Factory.h"

#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"


UMounteaAdvancedInventoryItemTemplate_Factory::UMounteaAdvancedInventoryItemTemplate_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UMounteaInventoryItemTemplate::StaticClass();
}

UObject* UMounteaAdvancedInventoryItemTemplate_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInventoryItemTemplate>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryItemTemplate_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Settings Config"));
	
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass
	(
		TitleText, ChosenClass, UMounteaInventoryItemTemplate::StaticClass()
	);

	if ( bPressedOk )
		ParentClass = ChosenClass;

	return bPressedOk;
}

bool UMounteaAdvancedInventoryItemTemplate_Factory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	return Extension.Equals(TEXT("mnteaitem"), ESearchCase::IgnoreCase) || 
		   Extension.Equals(TEXT("mnteaitems"), ESearchCase::IgnoreCase);
}

UObject* UMounteaAdvancedInventoryItemTemplate_Factory::FactoryCreateFile(UClass* InClass, UObject* InParent,
	const FName InName, const EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	TArray<UMounteaInventoryItemTemplate*> importedTemplates;
	FString errorMessage;
    
	if (!UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFile(importedTemplates, errorMessage))
	{
		if (Warn)
			Warn->Logf(ELogVerbosity::Error, TEXT("%s"), *errorMessage);
		bOutOperationCanceled = true;
		return nullptr;
	}

	if (importedTemplates.Num() == 0)
	{
		bOutOperationCanceled = true;
		return nullptr;
	}

	return importedTemplates[0];
}
