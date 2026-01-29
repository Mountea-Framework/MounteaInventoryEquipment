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
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Settings/MounteaAdvancedInventoryUIConfig.h"

void UMounteaAdvancedInventorySystemEditorStatics::OpenSettings(const FString& ContainerName,
                                                                const FString& CategoryName, const FString& SectionName)
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer(FName(ContainerName),  FName(CategoryName), FName(SectionName));
}

void UMounteaAdvancedInventorySystemEditorStatics::OpenAsset(const FString& AssetPath)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(AssetPath);
}

void UMounteaAdvancedInventorySystemEditorStatics::OpenInventoryConfig()
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	auto config = settings ? settings->AdvancedInventorySettingsConfig.LoadSynchronous() : nullptr;
	if (!IsValid(config))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Unable to locate the Mountea Inventory Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!")));
		return;
	}
	
	OpenAsset(config->GetPathName());
}

void UMounteaAdvancedInventorySystemEditorStatics::OpenEquipmentConfig()
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	auto config = settings ? settings->AdvancedEquipmentSettingsConfig.LoadSynchronous() : nullptr;
	if (!IsValid(config))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Unable to locate the Mountea Equipment Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!")));
		return;
	}

	OpenAsset(config->GetPathName());
}

void UMounteaAdvancedInventorySystemEditorStatics::OpenInventoryUIConfig()
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	auto config = settings ? settings->AdvancedInventoryUISettingsConfig.LoadSynchronous() : nullptr;
	if (!IsValid(config))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Unable to locate the Mountea Inventory UI Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!")));
		return;
	}
	
	OpenAsset(config->GetPathName());
}
