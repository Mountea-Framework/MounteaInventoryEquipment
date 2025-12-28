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
	FString fileContent;
	if (!FFileHelper::LoadFileToString(fileContent, *Filename))
	{
		if (Warn)
			Warn->Logf(ELogVerbosity::Error, TEXT("Failed to load file: %s"), *Filename);
		bOutOperationCanceled = true;
		return nullptr;
	}
    
	if (Filename.EndsWith(TEXT(".mnteaitems")))
	{
		TArray<FString> itemJsons;
		FString errorMessage;
        
		if (!UMounteaAdvancedInventoryItemTemplateEditorStatics::ParseMultipleTemplatesJson(fileContent, itemJsons, errorMessage))
		{
			if (Warn)
				Warn->Logf(ELogVerbosity::Error, TEXT("Failed to parse templates: %s"), *errorMessage);
			bOutOperationCanceled = true;
			return nullptr;
		}

		if (itemJsons.Num() == 0)
		{
			if (Warn)
				Warn->Logf(ELogVerbosity::Warning, TEXT("No templates found in file"));
			bOutOperationCanceled = true;
			return nullptr;
		}

		UMounteaInventoryItemTemplate* newTemplate = nullptr;
		for (int32 i = 0; i < itemJsons.Num(); ++i)
		{
			FName templateName = (i == 0) ? InName : FName(*FString::Printf(TEXT("%s_%d"), *InName.ToString(), i));
			UMounteaInventoryItemTemplate* resultTemplate = CreateSingleTemplate(InClass, InParent, templateName, Flags, itemJsons[i], Warn);
            
			if (i == 0)
				newTemplate = resultTemplate;
		}

		return newTemplate;
	}
	else
		return CreateSingleTemplate(InClass, InParent, InName, Flags, fileContent, Warn);
}

UMounteaInventoryItemTemplate* UMounteaAdvancedInventoryItemTemplate_Factory::CreateSingleTemplate(const UClass* InClass,
	UObject* InParent, const FName InName, const EObjectFlags Flags, const FString& JsonString, FFeedbackContext* Warn)
{
	TSharedPtr<FJsonObject> jsonObject;
	TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(JsonString);
    
	if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
	{
		if (Warn)
			Warn->Logf(ELogVerbosity::Error, TEXT("Invalid JSON format"));
		return nullptr;
	}

	UMounteaInventoryItemTemplate* resultTemplate = NewObject<UMounteaInventoryItemTemplate>(InParent, InClass, InName, Flags);
    
	FString errorMessage;
	if (!UMounteaAdvancedInventoryItemTemplateEditorStatics::DeserializeTemplateFromJson(jsonObject, resultTemplate, errorMessage))
	{
		if (Warn)
			Warn->Logf(ELogVerbosity::Error, TEXT("Failed to deserialize: %s"), *errorMessage);
		return nullptr;
	}

	return resultTemplate;
}
