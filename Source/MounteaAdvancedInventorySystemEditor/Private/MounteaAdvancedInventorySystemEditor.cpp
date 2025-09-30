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

#include "MounteaAdvancedInventorySystemEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "FileHelpers.h"
#include "IContentBrowserSingleton.h"
#include "ISettingsModule.h"
#include "AssetActions/MounteaAdvancedEquipmentComponent_AssetAction.h"
#include "AssetActions/MounteaAdvancedEquipmentSettingsConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryComponent_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryItemTemplate_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventorySettingsConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryThemeConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryUIComponent_AssetAction.h"
#include "Commands/FMAISCommands.h"
#include "Components/MounteaAttachmentContainerComponent.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "DetailsCustomizations/MounteaAttachmentContainerDetailsCustomization.h"
#include "Editor/SMounteaInventoryTemplateEditor.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "ToolMenus.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FMounteaAdvancedInventorySystemEditor"

static const FName MenuName("LevelEditor.LevelEditorToolBar.PlayToolBar");
const FName FMounteaAdvancedInventorySystemEditor::InventoryTemplateEditorTabId = TEXT("MounteaInventoryTemplateEditor");
TSharedPtr<SMounteaInventoryTemplateEditor> CurrentEditor;

void FMounteaAdvancedInventorySystemEditor::StartupModule()
{
	// Register Style
	{
		FMounteaAdvancedInventoryEditorStyle::Initialize();
	}
	
	// Register new Category
	{
		FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("MounteaAdvancedInventorySystem"), FText::FromString(TEXT("📦 Mountea Inventory & Equipment")));
	}

	// Thumbnails and Icons
	{
		AdvancedInventorySet = MakeShareable(new FSlateStyleSet("MounteaAdvancedInventorySet"));

		const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("MounteaAdvancedInventorySystem");

		if (PluginPtr.IsValid())
		{
			const FString ContentDir = IPluginManager::Get().FindPlugin("MounteaAdvancedInventorySystem")->GetBaseDir();
			
			// Advanced Inventory specific icons
			{
				AdvancedInventorySet->SetContentRoot(ContentDir);

				FSlateImageBrush* MounteaInventoryItemTemplateThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/MounteaInventoryItemTemplate"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* MounteaInventoryItemTemplateClassIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/MounteaInventoryItemTemplate"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (MounteaInventoryItemTemplateThumb && MounteaInventoryItemTemplateClassIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaInventoryItemTemplate", MounteaInventoryItemTemplateThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaInventoryItemTemplate", MounteaInventoryItemTemplateClassIcon);
				}
				
				FSlateImageBrush* MounteaAdvancedInventorySettingsConfigClassThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/MounteaAdvancedInventorySettingsConfig"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* MounteaAdvancedInventorySettingsConfigClassIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/MounteaAdvancedInventorySettingsConfig"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (MounteaAdvancedInventorySettingsConfigClassIcon && MounteaAdvancedInventorySettingsConfigClassThumb)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAdvancedInventorySettingsConfig", MounteaAdvancedInventorySettingsConfigClassThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAdvancedInventorySettingsConfig", MounteaAdvancedInventorySettingsConfigClassIcon);
				}

				FSlateImageBrush* MounteaAdvancedEquipmentSettingsConfigClassThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/MounteaAdvancedEquipmentSettingsConfig"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* MounteaAdvancedEquipmentSettingsConfigClassIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/MounteaAdvancedEquipmentSettingsConfig"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (MounteaAdvancedEquipmentSettingsConfigClassIcon && MounteaAdvancedEquipmentSettingsConfigClassThumb)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAdvancedEquipmentSettingsConfig", MounteaAdvancedEquipmentSettingsConfigClassThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAdvancedEquipmentSettingsConfig", MounteaAdvancedEquipmentSettingsConfigClassIcon);
				}

				FSlateImageBrush* MounteaAdvancedInventoryThemeConfigClassThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/ThemeConfigClassIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* MounteaAdvancedInventoryThemeConfigClassIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/ThemeConfigClassIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (MounteaAdvancedInventoryThemeConfigClassIcon && MounteaAdvancedInventoryThemeConfigClassThumb)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAdvancedInventoryThemeConfig", MounteaAdvancedInventoryThemeConfigClassThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAdvancedInventoryThemeConfig", MounteaAdvancedInventoryThemeConfigClassIcon);
				}

				FSlateImageBrush* MounteaAdvancedInventoryInteractiveWidgetConfigClassThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/InteractiveWidgetConfigIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* MounteaAdvancedInventoryInteractiveWidgetConfigClassIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/InteractiveWidgetConfigIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (MounteaAdvancedInventoryInteractiveWidgetConfigClassIcon && MounteaAdvancedInventoryInteractiveWidgetConfigClassThumb)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAdvancedInventoryInteractiveWidgetConfig", MounteaAdvancedInventoryInteractiveWidgetConfigClassThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAdvancedInventoryInteractiveWidgetConfig", MounteaAdvancedInventoryInteractiveWidgetConfigClassIcon);
				}

				FSlateImageBrush* AttachmentContainerThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/AttachmentContainerIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* AttachmentContainerIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/AttachmentContainerIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (AttachmentContainerThumb && AttachmentContainerIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAttachmentContainerComponent", AttachmentContainerThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAttachmentContainerComponent", AttachmentContainerIcon);
				}

				FSlateImageBrush* EquipmentComponentThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/EquipmentComponentIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* EquipmentComponentIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/EquipmentComponentIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (EquipmentComponentThumb && EquipmentComponentIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaEquipmentComponent", EquipmentComponentThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaEquipmentComponent", EquipmentComponentIcon);
				}

				FSlateImageBrush* InventoryComponentThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/InventoryComponentIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* InventoryComponentIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/InventoryComponentIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (InventoryComponentThumb && InventoryComponentIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaInventoryComponent", InventoryComponentThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaInventoryComponent", InventoryComponentIcon);
				}

				FSlateImageBrush* MounteaInventoryUIComponentThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/InventoryUIComponentIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* MounteaInventoryUIComponentClassIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/InventoryUIComponentIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (MounteaInventoryUIComponentThumb && MounteaInventoryUIComponentClassIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaInventoryUIComponent", MounteaInventoryUIComponentThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaInventoryUIComponent", MounteaInventoryUIComponentClassIcon);
				}

				FSlateImageBrush* AttachableComponentThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/AttachableComponentIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* AttachableComponentIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/AttachableComponentIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (AttachableComponentThumb && AttachableComponentIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAttachableComponent", AttachableComponentThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAttachableComponent", AttachableComponentIcon);
				}

				FSlateImageBrush* AttachableSlotThumb = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/AttachmentSlotIcon"), TEXT(".png")), FVector2D(128.f, 128.f));
				FSlateImageBrush* AttachableSlotIcon = new FSlateImageBrush(AdvancedInventorySet->RootToContentDir(TEXT("Resources/ClassIcons/AttachmentSlotIcon"), TEXT(".png")), FVector2D(16.f, 16.f));
				if (AttachableSlotThumb && AttachableSlotIcon)
				{
					AdvancedInventorySet->Set("ClassThumbnail.MounteaAdvancedAttachmentSlot", AttachableSlotThumb);
					AdvancedInventorySet->Set("ClassIcon.MounteaAdvancedAttachmentSlot", AttachableSlotIcon);
				}

				FSlateStyleRegistry::RegisterSlateStyle(*AdvancedInventorySet.Get());
			}

		}
	}

	// Asset Actions
	{
		AssetActions.Add(MakeShared<FMounteaAdvancedInventorySettingsConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryThemeConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedEquipmentSettingsConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryItemTemplate_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryComponent_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryUIComponent_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedEquipmentComponent_AssetAction>());

		for (const auto& Itr : AssetActions)
		{
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(Itr.ToSharedRef());
		}
	}

	// Register Menu Buttons
	{
		FMAISCommands::Register();

		PluginCommands = MakeShareable(new FUICommandList);
		
		IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
		mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::RegisterMenus));
	}

	// Register in Window tab
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("MounteaFramework");
			Section.Label = FText::FromString(TEXT("Mountea Framework"));
						
			FToolMenuEntry Entry = Section.AddMenuEntryWithCommandList
			(
				FMAISCommands::Get().PluginAction,
				PluginCommands,
				NSLOCTEXT("MounteaSupport", "TabTitle", "Mountea Support"),
				NSLOCTEXT("MounteaSupport", "TooltipText", "Opens Mountea Framework Support channel"),
				FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Help")
			);
		}
	}

	/*
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	{
		TArray<FOnGetDetailCustomizationInstance> CustomClassLayouts =
		{
			FOnGetDetailCustomizationInstance::CreateStatic(&FMounteaAttachmentContainerDetailsCustomization::MakeInstance),
		};
		RegisteredCustomClassLayouts =
		{
			UMounteaAttachmentContainerComponent::StaticClass()->GetFName(),
		};
		for (int32 i = 0; i < RegisteredCustomClassLayouts.Num(); i++)
		{
			PropertyModule.RegisterCustomClassLayout(RegisteredCustomClassLayouts[i], CustomClassLayouts[i]);
		}
	}
	*/
	// Register Tab Spawners
	{
		RegisterTabSpawners();
	}
}

void FMounteaAdvancedInventorySystemEditor::ShutdownModule()
{
	// Thumbnails and Icons
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(AdvancedInventorySet->GetStyleSetName());
	}

	// Asset Types Cleanup
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			for (const auto& Itr : AssetActions)
			{
				FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(Itr.ToSharedRef());
			}
		}
	}

	// Style Shutdown
	{
		FMounteaAdvancedInventoryEditorStyle::Shutdown();
	}

	// Help Button Cleanup
	{
		UToolMenus::UnRegisterStartupCallback(this);

		UToolMenus::UnregisterOwner(this);

		FMAISCommands::Unregister();
	}

	// Tab Spawners Cleanup
	{
		UnregisterTabSpawners();
	}
}

void FMounteaAdvancedInventorySystemEditor::RegisterTabSpawners()
{
	FGlobalTabmanager::Get()->RegisterTabSpawner(
		InventoryTemplateEditorTabId,
		FOnSpawnTab::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::SpawnInventoryTemplateEditorTab)
	)
	.SetDisplayName(LOCTEXT("InventoryTemplateEditorTabTitle", "Mountea Inventory Template Editor"))
	.SetTooltipText(LOCTEXT("InventoryTemplateEditorTooltipText", "Open the Mountea Inventory Template Editor"))
	.SetIcon(FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.InventorySystemIcon")); 
}

void FMounteaAdvancedInventorySystemEditor::UnregisterTabSpawners()
{
	if (FSlateApplication::IsInitialized())
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(InventoryTemplateEditorTabId);
}

void FMounteaAdvancedInventorySystemEditor::OpenInventoryTemplateEditor()
{
	FGlobalTabmanager::Get()->TryInvokeTab(InventoryTemplateEditorTabId);
}

TSharedRef<SDockTab> FMounteaAdvancedInventorySystemEditor::SpawnInventoryTemplateEditorTab(const FSpawnTabArgs& Args)
{
	TSharedRef<SMounteaInventoryTemplateEditor> newEditor = SNew(SMounteaInventoryTemplateEditor)
		.OnTemplateChanged_Lambda([](UMounteaInventoryItemTemplate* Template)
		{
			// Handle template changes if needed
			if (Template)
			{
				// Mark package as dirty
				Template->MarkPackageDirty();
			}
		});

	CurrentTemplateEditor = newEditor;

	TSharedRef<SDockTab> newTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("InventoryTemplateEditorTabTitle", "Mountea Inventory Template Editor"))
		.ToolTipText(LOCTEXT("InventoryTemplateEditorTooltipText", "Create and edit Mountea Inventory Item Templates"))
		[
			newEditor
		];

	return newTab;
}

void FMounteaAdvancedInventorySystemEditor::InventoryManagerButtonClicked() const
{
	const FString URL = "https://mountea-framework.github.io/InventoryManager/";

	if (!URL.IsEmpty())
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void FMounteaAdvancedInventorySystemEditor::LauncherButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaProjectLauncher";

	if (!URL.IsEmpty())
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void FMounteaAdvancedInventorySystemEditor::DialoguerButtonClicked() const
{
	const FString URL = "https://mountea-framework.github.io/MounteaDialoguer/";

	if (!URL.IsEmpty())
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void FMounteaAdvancedInventorySystemEditor::YoutubeButtonClicked() const
{
	const FString URL = "https://www.youtube.com/playlist?list=PLIU53wA8zZmgc1ty7R8WDHkYg037xT503";

	if (!URL.IsEmpty())
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void FMounteaAdvancedInventorySystemEditor::WikiButtonClicked() const
{
	const FString URL = "https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Getting-Started";

	if (!URL.IsEmpty())
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void FMounteaAdvancedInventorySystemEditor::PluginButtonClicked() const
{
	const FString URL = "https://discord.gg/waYT2cn37z";

	if (!URL.IsEmpty())
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void FMounteaAdvancedInventorySystemEditor::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	// Register in Window tab
	{
		if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help"))
		{
			if (Menu->ContainsSection("MounteaFramework") == false)
			{
				FToolMenuSection& Section = Menu->FindOrAddSection("MounteaFramework");
				
				Section.InsertPosition.Position = EToolMenuInsertType::First;
				Section.Label = FText::FromString(TEXT("Mountea Framework"));
				
				FToolMenuEntry SupportEntry = Section.AddMenuEntryWithCommandList
				(
					FMAISCommands::Get().PluginAction,
					PluginCommands,
					LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
					LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support channel"),
					FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Help")
				);
				SupportEntry.Name = FName("MounteaFrameworkSupport");
			}
		}
	}

	// Register in Level Editor Toolbar
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(MenuName);
		{
			FToolMenuSection& Entry = ToolbarMenu->FindOrAddSection("MounteaAdvancedInventoryFramework");
			FToolMenuEntry& MenuEntry = Entry.AddEntry(FToolMenuEntry::InitComboButton(
				"MounteaMenu",
				FUIAction(),
				FOnGetContent::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::MakeMounteaMenuWidget),
				LOCTEXT("MounteaMainMenu_Label", "Mountea Advanced Inventory Menu"),
				LOCTEXT("MounteaMainMenu_Tooltip", "📂 Open Mountea Advanced Inventory menu.\n\n❔ Provides link to Documentation, Support Discord and Dialogue tool."),
				FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.InventorySystemIcon"),
				false,
				"MounteaMenu"
			));
				
			MenuEntry.Label = LOCTEXT("MounteaMainMenu_Label", "Mountea Advanced Inventory Menu");
			MenuEntry.Name = TEXT("MounteaMenu");
			MenuEntry.StyleNameOverride = "CalloutToolbar";
			MenuEntry.SetCommandList(PluginCommands);
		}
	}
}

void FMounteaAdvancedInventorySystemEditor::SettingsButtonClicked() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project",  TEXT("Mountea Framework"), TEXT("Mountea Inventory System"));
}

void FMounteaAdvancedInventorySystemEditor::ConfigButtonClicked() const
{
	auto settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	auto config = settings ? settings->InventorySettingsConfig.LoadSynchronous() : nullptr;
	if (!IsValid(config))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Unable to locate the Mountea Inventory Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!")));
		return;
	}

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(config->GetPathName());
}

void FMounteaAdvancedInventorySystemEditor::EditorSettingsButtonClicked() const
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project",  TEXT("Mountea Framework"), TEXT("Mountea Inventory System (Editor)"));
}

TSharedRef<SWidget> FMounteaAdvancedInventorySystemEditor::MakeMounteaMenuWidget()
{
	FMenuBuilder MenuBuilder(true, PluginCommands);

	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Editor", "🎨 Mountea Item Templates Editor"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_TemplateEditorButton_Label", "Mountea Inventory Template Editor"),
			LOCTEXT("MounteaSystemEditor_TemplateEditorButton_ToolTip", "🎨 Open Mountea Inventory Template Editor\n\n❔ Create and edit Mountea Inventory Item Templates with a comprehensive visual editor. Design items with properties, assets, materials, and custom attributes.\n\n💡 Features include:\n- Visual template creation\n- Asset management\n- Property validation\n- Items import/export"),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.InventorySystemIcon"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::OpenInventoryTemplateEditor)
			)
		);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Content", "📦 Mountea Plugin Content"));
	MenuBuilder.AddMenuEntry(
				LOCTEXT("MounteaSystemEditor_OpenExampleLevel_Label", "Open Example Level"),
				LOCTEXT("MounteaSystemEditor_OpenExampleLevel_ToolTip", "🌄 Opens an example level demonstrating Mountea Advanced Inventory & Equipment System"),
				FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Level"),
				FUIAction(
					FExecuteAction::CreateLambda([]()
					{
						const FString mapPath = TEXT("/MounteaAdvancedInventorySystem/ExampleLevel/M_InventoryWorld");
						if (FPackageName::DoesPackageExist(mapPath))
							FEditorFileUtils::LoadMap(mapPath, false, true);
						else
						{
							FNotificationInfo notifInfo(LOCTEXT("MounteaSystemEditor_OpenExampleLevel_Notification", "Unable to open example level!"));
							notifInfo.bUseSuccessFailIcons = true;
							notifInfo.SubText = LOCTEXT("MounteaSystemEditor_OpenExampleLevel_Notification_SubText",
								"The example level is missing. Please, verify that the Mountea Advanced Inventory & Equipment plugin is installed correctly!");
							TSharedPtr<SNotificationItem> notificationItem = FSlateNotificationManager::Get().AddNotification(notifInfo);
							if (notificationItem.IsValid())
								notificationItem->SetCompletionState(SNotificationItem::CS_Fail);
						}
					})
				)
			);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("MounteaSystemEditor_OpenPluginFolder_Label", "Open Plugin Folder"),
		LOCTEXT("MounteaSystemEditor_OpenPluginFolder_ToolTip", "📂 Open the Mountea Advanced Inventory & Equipment plugin's folder"),
		FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Folder"),
		FUIAction(
			FExecuteAction::CreateLambda([]()
			{
				const FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
				TArray<FString> folderPaths;
				folderPaths.Add(TEXT("/MounteaAdvancedInventorySystem"));
				contentBrowserModule.Get().SetSelectedPaths(folderPaths, true);
			})
		)
	);
	MenuBuilder.EndSection();
	
	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Settings", "⚙ Mountea Advanced Inventory Settings"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_ConfigButton_Label", "Mountea Advanced Inventory Config"),
			LOCTEXT("MounteaSystemEditor_ConfigButton_ToolTip", "📄 Open Mountea Inventory Configuration\n\n❔ Define inventory types, rarities, categories, limits, UI layouts, and notification behaviors used by the core system."),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::ConfigButtonClicked)
			)
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_SettingsButton_Label", "Mountea Advanced Inventory Settings"),
			LOCTEXT("MounteaSystemEditor_SettingsButton_ToolTip", "⚙ Open Mountea Advanced Inventory Settings\n\n❔ Configure core Advanced Inventory system settings including default behaviors, performance options, and Advanced Inventory flow parameters. Customize your Advanced Inventory system's foundation here."),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Settings"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::SettingsButtonClicked)
			)
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_EditorSettingsButton_Label", "Mountea Advanced Inventory Editor Settings"),
			LOCTEXT("MounteaSystemEditor_EditorSettingsButton_ToolTip", "⚙ Open Mountea Advanced Inventory Editor Settings\n\n❔ Customize your Advanced Inventory editor experience with settings for:TBD\n\nAll settings are saved in DefaultMounteaSettings.ini"),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Settings"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::EditorSettingsButtonClicked)
			)
		);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("MounteaMenu_Links", LOCTEXT("MounteaMenuOptions_Options", "🌐 Mountea Links"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
			LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support Channel\n\n❔ Get direct assistance from our support team and community. Find solutions to common issues, share your experiences, and get help with implementation challenges. Join our active community of developers!"),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Help"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::PluginButtonClicked)
			)
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_WikiButton_Label", "Mountea Advanced Inventory Wiki"),
			LOCTEXT("MounteaSystemEditor_WikiButton_ToolTip", "📖 Open Mountea Advanced Inventory Documentation\n\n❔ Access comprehensive guides, tutorials, and API references. Find detailed examples, best practices, and advanced features to master the Mountea Advanced Inventory System. Your one-stop resource for all documentation needs."),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Wiki"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::WikiButtonClicked)
			)
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_YoutubeButton_Label", "Mountea Advanced Inventory Youtube"),
			LOCTEXT("MounteaSystemEditor_YoutubeButton_ToolTip", "👁️ Watch Mountea Advanced Inventory Youtube Videos\n\n❔ Visual learning resources featuring step-by-step tutorials, implementation guides, and practical examples. Perfect for both beginners and advanced users looking to expand their knowledge through video content."),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Youtube"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::YoutubeButtonClicked)
			)
		);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Tools", "⚒ Mountea Tools"));
	{
		MenuBuilder.AddMenuEntry(
		LOCTEXT("MounteaSystemEditor_InventoryManagerButton_Label", "Mountea Inventory Manager"),
		LOCTEXT("MounteaSystemEditor_InventoryManagerButton_ToolTip", "💼 Open Mountea Inventory Manager Tool\n\n❔ A browser-based Inventory Template Editor for Unreal Engine projects — manage item templates online from any device. Simple, fast, and accessible.\n\n💡 Features include:\n- Create and edit item templates\n- Manage item properties and categories\n- Export templates for use in Unreal Engine projects"),
		FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.InventoryManager"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::InventoryManagerButtonClicked)
		)
	);
		MenuBuilder.EndSection();
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MounteaSystemEditor_DialoguerButton_Label", "Mountea Dialoguer"),
			LOCTEXT("MounteaSystemEditor_DialoguerButton_ToolTip", "⛰ Open Mountea Dialoguer Standalone Tool\n\n❔ A powerful standalone Advanced Inventory crafting tool designed for narrative designers and writers. Create, edit, and manage complex Advanced Inventory trees with an intuitive interface. Seamlessly import your `.mnteadlg` files directly into the Mountea Advanced Inventory System.\n\n💡 Perfect for teams wanting to separate Advanced Inventory content creation from engine implementation."),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Dialoguer"),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::DialoguerButtonClicked)
			)
		);
	}
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("MounteaSystemEditor_LauncherButton_Label", "Mountea Project Launcher"),
		LOCTEXT("MounteaSystemEditor_LauncherButton_ToolTip", "🚀 Open Mountea Project Launcher\n\n❔ A versatile standalone tool for streamlined project testing and deployment. Launch your projects with customized configurations, test different build settings, and validate Advanced Inventory implementations in various environments.\n\n💡 Features include:\n- Multiple configuration profiles\n- Quick-launch presets\n- Custom command-line parameters\n- Integrated testing tools"),
		FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Launcher"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FMounteaAdvancedInventorySystemEditor::LauncherButtonClicked)
		)
	);
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaAdvancedInventorySystemEditor, MounteaAdvancedInventorySystemEditor)