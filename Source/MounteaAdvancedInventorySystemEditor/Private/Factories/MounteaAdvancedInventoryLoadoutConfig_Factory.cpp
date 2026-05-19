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

#include "MounteaAdvancedInventoryLoadoutConfig_Factory.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Statics/MounteaAdvancedInventoryLoadoutEditorStatics.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

UMounteaAdvancedInventoryLoadoutConfigs_Factory::UMounteaAdvancedInventoryLoadoutConfigs_Factory()
{
	bCreateNew    = true;
	bEditAfterNew = true;
	bEditorImport = true;

	Formats.Add(TEXT("mntealoadout;Mountea Loadout File"));
	Formats.Add(TEXT("mntealoadouts;Mountea Loadouts File"));

	SupportedClass = UMounteaAdvancedInventoryLoadoutConfig::StaticClass();
}

UObject* UMounteaAdvancedInventoryLoadoutConfigs_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMounteaAdvancedInventoryLoadoutConfig>(InParent, ParentClass, Name, Flags, Context);
}

bool UMounteaAdvancedInventoryLoadoutConfigs_Factory::ConfigureProperties()
{
	static const FText TitleText = FText::FromString(TEXT("Pick Parent Class for new Inventory & Equipment Loadouts Config"));

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = FMounteaAdvancedInventoryEditorUtilities::PickChildrenOfClass(TitleText, ChosenClass, UMounteaAdvancedInventoryLoadoutConfig::StaticClass());

	if (bPressedOk)
		ParentClass = ChosenClass;

	return bPressedOk;
}

bool UMounteaAdvancedInventoryLoadoutConfigs_Factory::FactoryCanImport(const FString& Filename)
{
	const FString ext = FPaths::GetExtension(Filename);
	return ext.Equals(TEXT("mntealoadout"), ESearchCase::IgnoreCase) ||
		   ext.Equals(TEXT("mntealoadouts"), ESearchCase::IgnoreCase);
}

UObject* UMounteaAdvancedInventoryLoadoutConfigs_Factory::FactoryCreateFile(
	UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	const FString targetFolder = InParent->GetPathName();

	TArray<UMounteaAdvancedInventoryLoadoutConfig*> importedLoadouts;
	FString errorMessage;

	if (!UMounteaAdvancedInventoryLoadoutEditorStatics::ImportLoadoutsFromFilePath(
		Filename, targetFolder, importedLoadouts, errorMessage))
	{
		if (Warn)
			Warn->Logf(ELogVerbosity::Error, TEXT("%s"), *errorMessage);
		bOutOperationCanceled = true;
		return nullptr;
	}

	return importedLoadouts.Num() > 0 ? importedLoadouts[0] : nullptr;
}

UObject* UMounteaAdvancedInventoryLoadoutConfigs_Factory::ImportObject(
	UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
{
	if (!FactoryCanImport(Filename))
		return nullptr;
	return FactoryCreateFile(InClass, InOuter, InName, Flags, Filename, Parms, nullptr, OutCanceled);
}
