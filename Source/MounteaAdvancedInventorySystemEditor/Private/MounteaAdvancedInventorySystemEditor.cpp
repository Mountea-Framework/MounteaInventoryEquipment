// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventorySystemEditor.h"

#include "AssetToolsModule.h"
#include "ISettingsModule.h"
#include "AssetActions/MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventorySettingsConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryThemeConfig_AssetAction.h"
#include "Commands/FMAISCommands.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventorySettingsConfig.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMounteaAdvancedInventorySystemEditor"

static const FName MenuName("LevelEditor.LevelEditorToolBar.PlayToolBar");

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

				//Register the created style
				FSlateStyleRegistry::RegisterSlateStyle(*AdvancedInventorySet.Get());
			}
		}
	}

	// Asset Actions
	{
		AssetActions.Add(MakeShared<FMounteaAdvancedInventorySettingsConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryThemeConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction>());

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

TSharedRef<SWidget> FMounteaAdvancedInventorySystemEditor::MakeMounteaMenuWidget() const
{
	FMenuBuilder MenuBuilder(true, PluginCommands);

	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Settings", "Mountea Advanced Inventory Settings"));
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

	MenuBuilder.BeginSection("MounteaMenu_Links", LOCTEXT("MounteaMenuOptions_Options", "Mountea Links"));
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

	MenuBuilder.BeginSection("MounteaMenu_Tools", LOCTEXT("MounteaMenuOptions_Tools", "Mountea Tools"));
	{
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