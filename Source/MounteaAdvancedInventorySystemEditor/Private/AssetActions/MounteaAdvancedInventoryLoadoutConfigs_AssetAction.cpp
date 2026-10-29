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

#include "MounteaAdvancedInventoryLoadoutConfigs_AssetAction.h"

#include "MounteaAdvancedInventoryThemeConfig_AssetAction.h"
#include "Definitions/MounteaAdvancedInventoryEditorBaseTypes.h"
#include "Definitions/MounteaAdvancedInventoryLoadoutConfig.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Helpers/MounteaEditorNotificationHelpers.h"
#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"
#include "Statics/MounteaAdvancedInventoryLoadoutEditorStatics.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"


#define LOCTEXT_NAMESPACE "MounteaAdvancedInventoryLoadoutConfigs_AssetAction"

FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::FMounteaAdvancedInventoryLoadoutConfigs_AssetAction()
{
}

void FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetActions(
    const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	const auto selection = GetTypedWeakObjectPtrs<UMounteaAdvancedInventoryLoadoutConfig>(InObjects);
	MenuBuilder.AddMenuEntry(LOCTEXT("ExportLoadouts", "Export Loadout(s)..."),
	    LOCTEXT("ExportLoadoutsTip", "Export selected loadouts using the shared Mountea ZIP format."),
	    FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Export"),
	    FUIAction(FExecuteAction::CreateLambda([selection]()
	{
		TArray<UMounteaAdvancedInventoryLoadoutConfig*> loadouts;

		for (const auto& loadout : selection)
		{
			if (loadout.IsValid())
				loadouts.Add(loadout.Get());
		}

		if (loadouts.IsEmpty())
			return;

		const bool bundle = loadouts.Num() > 1;
		const FString path = UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowSaveFileDialog(
		    TEXT("Export Loadouts"), bundle ? TEXT("Loadouts") : loadouts[0]->GetName(),
		    bundle ? TEXT("Mountea Loadouts (*.mntealoadouts)|*.mntealoadouts")
		           : TEXT("Mountea Loadout (*.mntealoadout)|*.mntealoadout"));

		if (path.IsEmpty())
			return;

		FString error;
		const bool success =
		    UMounteaAdvancedInventoryLoadoutEditorStatics::ExportLoadoutsToFilePath(loadouts, path, error);
		FMounteaEditorNotificationHelpers::ShowNotification(success ? TEXT("Loadouts exported successfully.") : error,
		    success ? EMounteaEditorNotificationType::Success : EMounteaEditorNotificationType::Error);
	})));
}

FText FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetName() const
{
	return LOCTEXT("MounteaAdvancedInventoryLoadoutConfigs_AssetAction", "Mountea Advanced Inventory Loadout Config");
}

FColor FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetTypeColor() const
{
	return FColor::White;
}

UClass* FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetSupportedClass() const
{
	return UMounteaAdvancedInventoryLoadoutConfig::StaticClass();
}

uint32 FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"));
	}

	return EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaAdvancedInventoryLoadoutConfigs_AssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		MounteaAdvancedInventoryBaseTypes::LoadoutClassesMenuEntry
	};
	return AssetTypeActionSubMenu;
}


#undef LOCTEXT_NAMESPACE
