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

#include "MounteaAdvancedInventoryWorkspace_Factory.h"

#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Helpers/MounteaEditorNotificationHelpers.h"
#include "Statics/MounteaAdvancedInventoryWorkspaceEditorStatics.h"

UMounteaAdvancedInventoryWorkspace_Factory::UMounteaAdvancedInventoryWorkspace_Factory()
{
	bCreateNew    = false;
	bEditAfterNew = true;
	bEditorImport = true;

	Formats.Add(TEXT("mnteainventory;Mountea Workspace File"));

	SupportedClass = UMounteaInventoryItemTemplate::StaticClass();
}

bool UMounteaAdvancedInventoryWorkspace_Factory::FactoryCanImport(const FString& Filename)
{
	const FString ext = FPaths::GetExtension(Filename);
	return ext.Equals(TEXT("mnteainventory"), ESearchCase::IgnoreCase);
}

UObject* UMounteaAdvancedInventoryWorkspace_Factory::FactoryCreateFile(
	UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	const FString targetRootFolder = InParent->GetPathName();

	TArray<UMounteaInventoryItemTemplate*> importedItems;
	TArray<UMounteaRecipeTemplate*> importedRecipes;
	TArray<UMounteaAdvancedInventoryLoadoutConfig*> importedLoadouts;
	FString errorMessage;

	if (!UMounteaAdvancedInventoryWorkspaceEditorStatics::ImportWorkspaceFromFilePath(
		Filename, targetRootFolder, importedItems, importedRecipes, importedLoadouts, errorMessage))
	{
		if (Warn)
			Warn->Logf(ELogVerbosity::Error, TEXT("%s"), *errorMessage);
		FMounteaEditorNotificationHelpers::ShowNotification(errorMessage, EMounteaEditorNotificationType::Error);
		bOutOperationCanceled = true;
		return nullptr;
	}

	FMounteaEditorNotificationHelpers::ShowNotification(
		FString::Printf(TEXT("Workspace import complete: %d items, %d recipes, %d loadouts"),
			importedItems.Num(), importedRecipes.Num(), importedLoadouts.Num()),
		EMounteaEditorNotificationType::Success);

	if (importedItems.Num() > 0)
		return importedItems[0];
	if (importedRecipes.Num() > 0)
		return importedRecipes[0];
	if (importedLoadouts.Num() > 0)
		return importedLoadouts[0];
	return nullptr;
}

UObject* UMounteaAdvancedInventoryWorkspace_Factory::ImportObject(
	UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
{
	if (!FactoryCanImport(Filename))
		return nullptr;
	return FactoryCreateFile(InClass, InOuter, InName, Flags, Filename, Parms, nullptr, OutCanceled);
}
