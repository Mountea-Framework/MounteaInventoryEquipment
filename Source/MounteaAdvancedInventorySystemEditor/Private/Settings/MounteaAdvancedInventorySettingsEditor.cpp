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


#include "Settings/MounteaAdvancedInventorySettingsEditor.h"

#include "Definitions/MounteaAdvancedInventoryEditorTypes.h"
#include "Interfaces/IPluginManager.h"

UMounteaAdvancedInventorySettingsEditor::UMounteaAdvancedInventorySettingsEditor() : bDisplayEditorButtonText(false)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory System (Editor)");
	
	auto ResolvePluginPath = [](const FString& RelativePath) -> FString
	{
		const FString pluginDir =
			IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"))->GetBaseDir();

		const FString fullPath = FPaths::Combine(pluginDir, RelativePath);
		return FPaths::ConvertRelativePathToFull(fullPath);
	};

	SharedStylesheetPath.FilePath = ResolvePluginPath(TEXT("Resources/Help/ItemTemplateEditor.css"));
	SharedScriptPath.FilePath = ResolvePluginPath(TEXT("Resources/Help/ItemTemplateEditor.js"));
	
	FItemTemplateEditorPageConfig firstPage(
	NSLOCTEXT("MounteaAdvancedInventorySettingsEditor", "EditorTemplatePages_FirstPage", "Introduction"),
	ResolvePluginPath(TEXT("Resources/Help/page_0.html"))
);

	FItemTemplateEditorPageConfig secondPage(
		NSLOCTEXT("MounteaAdvancedInventorySettingsEditor", "EditorTemplatePages_SecondPage", "Configuration"),
		ResolvePluginPath(TEXT("Resources/Help/page_1.html"))
	);

	FItemTemplateEditorPageConfig thirdPage(
		NSLOCTEXT("MounteaAdvancedInventorySettingsEditor", "EditorTemplatePages_ThirdPage", "Create New Template"),
		ResolvePluginPath(TEXT("Resources/Help/page_2.html"))
	);

	FItemTemplateEditorPageConfig fourthPage(
		NSLOCTEXT("MounteaAdvancedInventorySettingsEditor", "EditorTemplatePages_FourthPage", "Exporting Templates"),
		ResolvePluginPath(TEXT("Resources/Help/page_3.html"))
	);

	FItemTemplateEditorPageConfig fifthPage(
		NSLOCTEXT("MounteaAdvancedInventorySettingsEditor", "EditorTemplatePages_FifthPage", "Importing Templates"),
		ResolvePluginPath(TEXT("Resources/Help/page_4.html"))
	);

	FItemTemplateEditorPageConfig sixthPage(
		NSLOCTEXT("MounteaAdvancedInventorySettingsEditor", "EditorTemplatePages_SixthPage", "Advanced Tips"),
		ResolvePluginPath(TEXT("Resources/Help/page_5.html"))
	);

	EditorTemplatePages.Add(0, firstPage);
	EditorTemplatePages.Add(1, secondPage);
	EditorTemplatePages.Add(2, thirdPage);
	EditorTemplatePages.Add(3, fourthPage);
	EditorTemplatePages.Add(4, fifthPage);
	EditorTemplatePages.Add(5, sixthPage);

}
