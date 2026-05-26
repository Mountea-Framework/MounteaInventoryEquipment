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


#include "Statics/MounteaAdvancedInventorySystemEditorStatics.h"

#include "ISettingsModule.h"
#include "Misc/MessageDialog.h"

void UMounteaAdvancedInventorySystemEditorStatics::OpenSettings(const FString& ContainerName,  const FString& CategoryName, const FString& SectionName)
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer(FName(ContainerName),  FName(CategoryName), FName(SectionName));
}

void UMounteaAdvancedInventorySystemEditorStatics::OpenAsset(const FString& AssetPath)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(AssetPath);
}

void UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(const FSoftObjectPath& ConfigPath, FText& OutMessage, const FText& ErrorMessage)
{
	OutMessage = FText::GetEmpty();

	UObject* config = ConfigPath.TryLoad();
	if (!IsValid(config))
	{
		OutMessage = ErrorMessage;
		FMessageDialog::Open(EAppMsgType::Ok, OutMessage);
		return;
	}

	UMounteaAdvancedInventorySystemEditorStatics::OpenAsset(config->GetPathName());
}
