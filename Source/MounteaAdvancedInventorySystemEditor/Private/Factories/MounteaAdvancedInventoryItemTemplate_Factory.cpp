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
#include "Subsystems/MounteaInventoryTemplateEditorSubsystem.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"


UMounteaAdvancedInventoryItemTemplate_Factory::UMounteaAdvancedInventoryItemTemplate_Factory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	bEditorImport = true;
	Formats.Add("mnteaitem;Mountea Item Template File");
	Formats.Add("mnteaitems;Mountea Item Templates File");
	SupportedClass = UMounteaInventoryItemTemplate::StaticClass();
}

UObject* UMounteaAdvancedInventoryItemTemplate_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, const FName Name,
	const EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaInventoryItemTemplate>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryItemTemplate_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Mountea Inventory Item Template"));
	
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

bool UMounteaAdvancedInventoryItemTemplate_Factory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (Obj == nullptr) return false;
	UMounteaInventoryItemTemplate* itemTemplate = Cast<UMounteaInventoryItemTemplate>(Obj);
	if (itemTemplate == nullptr) return false;
	
	return true;
}

EReimportResult::Type UMounteaAdvancedInventoryItemTemplate_Factory::Reimport(UObject* Obj)
{
	bool bReimportSuccess = false;
	return bReimportSuccess ? EReimportResult::Succeeded : EReimportResult::Failed;
}

void UMounteaAdvancedInventoryItemTemplate_Factory::PostImportCleanUp()
{
	FReimportHandler::PostImportCleanUp();
	
	ReimportPaths.Empty();
}

UObject* UMounteaAdvancedInventoryItemTemplate_Factory::FactoryCreateFile(UClass* InClass, UObject* InParent,
	const FName InName, const EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	const FString targetFolder = InParent->GetPathName();
    
	TArray<UMounteaInventoryItemTemplate*> importedTemplates;
	FString errorMessage;
    
	if (!UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFilePath(Filename, targetFolder, importedTemplates, errorMessage))
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

	if (UMounteaInventoryTemplateEditorSubsystem* editorTemplateSubsystem = GEditor->GetEditorSubsystem<UMounteaInventoryTemplateEditorSubsystem>())
		editorTemplateSubsystem->NotifyTemplatesChanged();

	return importedTemplates[0];
}

UObject* UMounteaAdvancedInventoryItemTemplate_Factory::ImportObject(UClass* InClass, UObject* InOuter, const FName InName,
	const EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
{
	if (!FactoryCanImport(Filename))
		return nullptr;
	return FactoryCreateFile(InClass, InOuter, InName, Flags, Filename, Parms, nullptr, OutCanceled);
}
