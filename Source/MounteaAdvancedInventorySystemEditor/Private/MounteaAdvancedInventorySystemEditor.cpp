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

#include "AssetActions/MounteaAdvancedEquipmentComponent_AssetAction.h"
#include "AssetActions/MounteaAdvancedEquipmentSettingsConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryComponent_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryGlobalConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryInteractiveWidgetConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryInteractiveWidgetEnvConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryItemTemplate_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryCraftingUIConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventorySelectableInventoryItemAction_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventorySettingsConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryThemeConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryUIComponent_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryUIConfig_AssetAction.h"

#include "Commands/FMAISCommands.h"

#include "Customizations/MounteaJsonObjectDefinitionFieldCustomization.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "PropertyEditorModule.h"

#include "Editor/SMounteaInventoryTemplateEditor.h"

#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#include "Framework/Docking/TabManager.h"
#include "Framework/Notifications/NotificationManager.h"

#include "Statics/MounteaAdvancedInventorySystemEditorStatics.h"
#include "Visualiser/MounteaEquipmentComponentVisualizer.h"

#include "Widgets/Notifications/SNotificationList.h"
#include "Components/MounteaAttachmentContainerComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "AssetActions/MounteaAdvancedInventoryCraftingConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryGlobalUIConfig_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryLoadoutComponent_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryLoadoutConfigs_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryLoadoutItem_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryModalsDataTable_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryRecipeIngredientsList_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryRecipeIngredient_AssetAction.h"
#include "AssetActions/MounteaAdvancedInventoryRecipeTemplate_AssetAction.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

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
		const TSharedPtr<IPlugin> pluginPtr = IPluginManager::Get().FindPlugin("MounteaAdvancedInventorySystem");

		if (pluginPtr.IsValid())
		{
			{
				const FString contentDir = pluginPtr->GetBaseDir();
				AdvancedInventorySet->SetContentRoot(contentDir);

				struct FClassIconMapping
				{
					const TCHAR* ClassName;
					const TCHAR* ResourceName;
				};

				const FClassIconMapping classIconMappings[] =
				{
					{ TEXT("MounteaInventoryItemTemplate"), TEXT("MounteaInventoryItemTemplate") },
					{ TEXT("MounteaAdvancedInventorySettingsConfig"), TEXT("MounteaAdvancedInventorySettingsConfig") },
					{ TEXT("MounteaAdvancedEquipmentSettingsConfig"), TEXT("MounteaAdvancedEquipmentSettingsConfig") },
					{ TEXT("MounteaAdvancedInventoryThemeConfig"), TEXT("ThemeConfigClassIcon") },
					{ TEXT("MounteaAdvancedInventoryInteractiveWidgetConfig"), TEXT("InteractiveWidgetConfigIcon") },
					{ TEXT("MounteaAttachmentContainerComponent"), TEXT("AttachmentContainerIcon") },
					{ TEXT("MounteaEquipmentComponent"), TEXT("EquipmentComponentIcon") },
					{ TEXT("MounteaInventoryComponent"), TEXT("InventoryComponentIcon") },
					{ TEXT("MounteaInventoryUIComponent"), TEXT("InventoryUIComponentIcon") },
					{ TEXT("MounteaAttachableComponent"), TEXT("AttachableComponentIcon") },
					{ TEXT("MounteaAdvancedAttachmentSlot"), TEXT("AttachmentSlotIcon") },
					{ TEXT("MounteaAdvancedInventoryUIConfig"), TEXT("MounteaAdvancedInventoryUISettingsConfig") },
					{ TEXT("MounteaAdvancedInventoryPreviewEnvironmentSettings"), TEXT("PreviewEnvironmentSettingsIcon") },
					{ TEXT("MounteaSelectableInventoryItemAction"), TEXT("ItemActionIcon") },
					{ TEXT("MounteaCallbackInventoryItemAction"), TEXT("ItemActionCallbackIcon") },
					{ TEXT("MounteaAdvancedInventoryLoadoutConfig"), TEXT("LoadoutConfiguration") },
					{ TEXT("MounteaAdvancedInventoryLoadoutComponent"), TEXT("LoadoutComponent") },
					{ TEXT("MounteaAdvancedInventoryLoadoutItem"), TEXT("LoadoutItem") },
					{ TEXT("MounteaRecipeTemplate"), TEXT("RecipeBook") },
					{ TEXT("MounteaRecipeIngredientsList"), TEXT("RecipeIngredientsList") },
					{ TEXT("MounteaRecipeIngredient"), TEXT("MounteaRecipeIngredients") },
					{ TEXT("MounteaCraftingParticipantComponent"), TEXT("CraftingParticipantIcon") },
					{ TEXT("MounteaCraftingParticipantUIComponent"), TEXT("CraftingParticipantUIIcon") },
					{ TEXT("MounteaCraftingStationComponent"), TEXT("CraftingStationIcon") },
					{ TEXT("MounteaAdvancedCraftingConfig"), TEXT("CraftingConfigIcon") },
					{ TEXT("MounteaAdvancedCraftingUIConfig"), TEXT("CraftingUIConfigIcon") },
					{ TEXT("MounteaAdvancedInventoryGlobalConfig"), TEXT("GlobalUIConfig") },
					{ TEXT("MounteaAdvancedInventoryGlobalUIConfig"), TEXT("GlobalUIConfig") },
					{ TEXT("MounteaAdvancedInventoryModalsDataTable"), TEXT("ModalsDataTableIcon") }
				};

				const auto RegisterClassIcon = [this](const TCHAR* ClassName, const TCHAR* ResourceName)
				{
					const FString resourcePath = FString::Printf(TEXT("Resources/ClassIcons/%s"), ResourceName);
					const FString brushPath = AdvancedInventorySet->RootToContentDir(*resourcePath, TEXT(".png"));
					const FString thumbnailKey = FString::Printf(TEXT("ClassThumbnail.%s"), ClassName);
					const FString iconKey = FString::Printf(TEXT("ClassIcon.%s"), ClassName);

					AdvancedInventorySet->Set(*thumbnailKey, new FSlateImageBrush(brushPath, FVector2D(128.f, 128.f)));
					AdvancedInventorySet->Set(*iconKey, new FSlateImageBrush(brushPath, FVector2D(16.f, 16.f)));
				};

				for (const FClassIconMapping& classIconMapping : classIconMappings)
				{
					RegisterClassIcon(classIconMapping.ClassName, classIconMapping.ResourceName);
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
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryUIConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventorySelectableInventoryItemAction_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryCallbackInventoryItemAction_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryInteractiveWidgetEnvironmentConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryLoadoutConfigs_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryLoadoutItem_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryLoadoutComponent_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryModalsDataTable_AssetAction>());
		
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryRecipeTemplate_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryRecipeIngredientsList_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryRecipeIngredient_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryCraftingConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryCraftingUIConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryGlobalConfig_AssetAction>());
		AssetActions.Add(MakeShared<FMounteaAdvancedInventoryGlobalUIConfig_AssetAction>());
		
		for (const auto& Itr : AssetActions)
		{
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(Itr.ToSharedRef());
		}
	}

	// Detail Customizations
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		RegisteredCustomPropertyTypeLayouts =
		{
			TEXT("MounteaJsonObjectDefinitionField")
		};

		PropertyModule.RegisterCustomPropertyTypeLayout(
			RegisteredCustomPropertyTypeLayouts[0],
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMounteaJsonObjectDefinitionFieldCustomization::MakeInstance)
		);
		PropertyModule.NotifyCustomizationModuleChanged();
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
				FMAISCommands::Get().MAI_MounteaSupportAction,
				PluginCommands,
				NSLOCTEXT("MounteaSupport", "TabTitle", "Mountea Support"),
				NSLOCTEXT("MounteaSupport", "TooltipText", "🆘 Open Mountea Framework Support channel"),
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

	// Register Equipment Component Visualiser
	{
		if (GUnrealEd)
		{
			TSharedPtr<FComponentVisualizer> Visualizer = MakeShareable(new FMounteaEquipmentComponentVisualizer);
			GUnrealEd->RegisterComponentVisualizer(UMounteaAttachmentContainerComponent::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}
	}
	
	// Validate WebBrowserWidget
	{
		if (!FModuleManager::Get().IsModuleLoaded("WebBrowserWidget"))
		{
			FText errorTitle = NSLOCTEXT("MounteaInventory", "WebBrowserMissing", "⚠ Missing Plugin ⚠");
			FText errorMessage = NSLOCTEXT("MounteaInventory", "WebBrowserMissingMessage", 
				"The 'Web Browser Widget' plugin is required for the Item Template Editor system.\n\n"
				"We need to enable the plugin and restart the Editor. Sorry for the inconvenience.");
		
			FMessageDialog::Open(EAppMsgType::Ok, errorMessage, errorTitle);
		
			FString projectFilePath = FPaths::ProjectDir() / FApp::GetProjectName() + TEXT(".uproject");			
			FString jsonString;
			if (FFileHelper::LoadFileToString(jsonString, *projectFilePath))
			{
				TSharedPtr<FJsonObject> jsonObject;
				TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonString);
				
				if (FJsonSerializer::Deserialize(reader, jsonObject) && jsonObject.IsValid())
				{
					TArray<TSharedPtr<FJsonValue>> pluginsArray;
					if (jsonObject->HasField(TEXT("Plugins")))
						pluginsArray = jsonObject->GetArrayField(TEXT("Plugins"));
					
					bool bExists = false;
					for (const TSharedPtr<FJsonValue>& plugin : pluginsArray)
					{
						TSharedPtr<FJsonObject> pluginObj = plugin->AsObject();
						if (pluginObj.IsValid() && pluginObj->GetStringField(TEXT("Name")) == TEXT("WebBrowserWidget"))
						{
							pluginObj->SetBoolField(TEXT("Enabled"), true);
							bExists = true;
							break;
						}
					}
					
					if (!bExists)
					{
						TSharedPtr<FJsonObject> newPlugin = MakeShareable(new FJsonObject());
						newPlugin->SetStringField(TEXT("Name"), TEXT("WebBrowserWidget"));
						newPlugin->SetBoolField(TEXT("Enabled"), true);
						pluginsArray.Add(MakeShareable(new FJsonValueObject(newPlugin)));
					}
					
					jsonObject->SetArrayField(TEXT("Plugins"), pluginsArray);
					
					FString outputString;
					TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&outputString);
					if (FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer))
					{
						if (FFileHelper::SaveStringToFile(outputString, *projectFilePath))
							FUnrealEdMisc::Get().RestartEditor(false);
					}
				}
			}
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

	// Detail Customizations Cleanup
	{
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			for (const FName& layoutName : RegisteredCustomPropertyTypeLayouts)
				PropertyModule.UnregisterCustomPropertyTypeLayout(layoutName);

			PropertyModule.NotifyCustomizationModuleChanged();
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

	// Equipment Component Visualiser Cleanup
	{
		if (GUnrealEd)
			GUnrealEd->UnregisterComponentVisualizer(UMounteaAttachmentContainerComponent::StaticClass()->GetFName());
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
	.SetIcon(FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.TemplatesEditor")); 
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
		.Label(LOCTEXT("InventoryTemplateEditorTabTitle", "Inventory Template Editor"))
		.ToolTipText(LOCTEXT("InventoryTemplateEditorTooltipText", "Create and edit Mountea Inventory Item Templates"))
		[
			newEditor
		];
	
	newTab->SetTabIcon(FAppStyle::GetBrush("MAISStyleSet.TemplatesEditor"));

	return newTab;
}

void FMounteaAdvancedInventorySystemEditor::RegisterMenus()
{
	if (!UToolMenus::Get()->IsMenuRegistered(MounteaAdvancedInventoryToolbar::MounteaSharedMenuName))
		UToolMenus::Get()->RegisterMenu(MounteaAdvancedInventoryToolbar::MounteaSharedMenuName, NAME_None, EMultiBoxType::Menu, false);

	UToolMenu* toolbarMenu = UToolMenus::Get()->ExtendMenu(MenuName);
	FToolMenuSection& sharedSection = toolbarMenu->FindOrAddSection(MounteaAdvancedInventoryToolbar::MounteaSharedSectionName);
	sharedSection.Label = LOCTEXT("MounteaSharedSection_Label", "Mountea Framework");

	if (sharedSection.FindEntry(MounteaAdvancedInventoryToolbar::MounteaSharedButtonName) == nullptr)
	{
		FToolMenuEntry comboButton = FToolMenuEntry::InitComboButton(
			MounteaAdvancedInventoryToolbar::MounteaSharedButtonName,
			FUIAction(),
			FOnGetContent::CreateLambda([SharedMenuName = MounteaAdvancedInventoryToolbar::MounteaSharedMenuName, CmdList = PluginCommands]() -> TSharedRef<SWidget>
			{
				return UToolMenus::Get()->GenerateWidget(SharedMenuName, FToolMenuContext(CmdList));
			}),
			LOCTEXT("MounteaMainMenu_Label", "Mountea Framework"),
			LOCTEXT("MounteaMainMenu_Tooltip", "📂 Open Mountea Framework menu.\n\n❔ Provides access to all Mountea Framework tools, documentation, and support."),
			FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.MounteaLogo"),
			false,
			MounteaAdvancedInventoryToolbar::MounteaSharedButtonName
		);
		comboButton.StyleNameOverride = "CalloutToolbar";
		sharedSection.AddEntry(comboButton);
	}

	FToolMenuOwnerScoped OwnerScoped(this);
	{
		// Register in Help menu
		if (UToolMenu* toolMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help"))
		{
			if (toolMenu->ContainsSection("MounteaFramework") == false)
			{
				FToolMenuSection& mounteaSection = toolMenu->FindOrAddSection("MounteaFramework");
				mounteaSection.InsertPosition.Position = EToolMenuInsertType::First;
				mounteaSection.Label = FText::FromString(TEXT("Mountea Framework"));
				FToolMenuEntry supportEntry = mounteaSection.AddMenuEntryWithCommandList(
					FMAISCommands::Get().MAI_MounteaSupportAction,
					PluginCommands,
					LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
					LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support channel"),
					FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Help")
				);
				supportEntry.Name = FName("MounteaFrameworkSupport");
			}
		}

		// Extend shared submenu with a nested Inventory submenu entry
		UToolMenu* sharedMenu = UToolMenus::Get()->ExtendMenu(MounteaAdvancedInventoryToolbar::MounteaSharedMenuName);
		{
			FToolMenuSection& invSection = sharedMenu->FindOrAddSection("MounteaInventory");
			invSection.Label = LOCTEXT("InvSection_Label", "Mountea Advanced Inventory & Equipment");
			invSection.AddEntry(FToolMenuEntry::InitSubMenu(
				"MounteaInventorySubMenu",
				LOCTEXT("InvSubMenu_Label", "Mountea Advanced Inventory"),
				LOCTEXT("InvSubMenu_Tooltip", "📦 Mountea Advanced Inventory & Equipment tools"),
				FNewToolMenuDelegate::CreateLambda([this](UToolMenu* subMenu)
				{
					{
						FToolMenuSection& toolsSection = subMenu->FindOrAddSection("MounteaInventory_Tools");
						toolsSection.Label = LOCTEXT("InvTools_Label", "Mountea Item Templates Editor");
						toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_TemplateEditor",
							LOCTEXT("MounteaSystemEditor_TemplateEditorButton_Label", "Mountea Inventory Template Editor"),
							LOCTEXT("MounteaSystemEditor_TemplateEditorButton_ToolTip", "🎨 Open Mountea Inventory Template Editor\n\n❔ Create and edit Mountea Inventory Item Templates with a comprehensive visual editor. Design items with properties, assets, materials, and custom attributes.\n\n💡 Features include:\n- Visual template creation\n- Asset management\n- Property validation\n- Items import/export"),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.TemplatesEditor"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&) { OpenInventoryTemplateEditor(); })
						));
					}

					{
						FToolMenuSection& contentSection = subMenu->FindOrAddSection("MounteaInventory_Content");
						contentSection.Label = LOCTEXT("InvContent_Label", "Mountea Plugin Content");
						contentSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_ExampleLevel",
							LOCTEXT("MounteaSystemEditor_OpenExampleLevel_Label", "Open Example Level"),
							LOCTEXT("MounteaSystemEditor_OpenExampleLevel_ToolTip", "🌄 Opens an example level demonstrating Mountea Advanced Inventory & Equipment System"),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Level"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
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
						));
						contentSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_PluginFolder",
							LOCTEXT("MounteaSystemEditor_OpenPluginFolder_Label", "Open Plugin Folder"),
							LOCTEXT("MounteaSystemEditor_OpenPluginFolder_ToolTip", "📂 Open the Mountea Advanced Inventory & Equipment plugin's folder"),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Folder"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
							{
								const FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
								TArray<FString> folderPaths;
								folderPaths.Add(TEXT("/MounteaAdvancedInventorySystem"));
								contentBrowserModule.Get().SetSelectedPaths(folderPaths, true);
							})
						));
					}

					{
						FToolMenuSection& configSection = subMenu->FindOrAddSection("MounteaInventory_Config");
						configSection.Label = LOCTEXT("InvConfig_Label", "Mountea Advanced Inventory Configs");
						
						// Global UI Config
						configSection.AddEntry(FToolMenuEntry::InitSubMenu(
							"MounteaInventory_Config_GlobalConfigSubMenu",
							LOCTEXT("MounteaSystemEditor_GlobalConfigSubMenu_Label", "Shared Config"),
							LOCTEXT("MounteaSystemEditor_GlobalConfigSubMenu_ToolTip", "📄 Open Mountea Inventory & Equipment shared configuration menus."),
							FNewToolMenuDelegate::CreateLambda([](UToolMenu* globalConfigSubMenu)
							{
								FToolMenuSection& inventoryConfigSection = globalConfigSubMenu->FindOrAddSection("MounteaInventory_Config_GlobalConfig");
								inventoryConfigSection.Label = LOCTEXT("InvConfigGlobalConfig_Label", "Shared Configs");
								inventoryConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_GlobalConfig",
									LOCTEXT("MounteaSystemEditor_GlobalInventoryConfigButton_Label", "Global Config"),
									LOCTEXT("MounteaSystemEditor_GlobalInventoryConfigButton_ToolTip", "📄 Open Mountea Inventory & Equipment Global Configuration\n\n❔ Define shared data definitions, including named JSON object definitions used for JSON generation and validation."),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->GlobalConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_GlobalInventoryConfigButton_Error", "Unable to locate the Mountea Inventory Global Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
								inventoryConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_Config",
									LOCTEXT("MounteaSystemEditor_GlobalConfigButton_Label", "Global UI Config"),
									LOCTEXT("MounteaSystemEditor_GlobalConfigButton_ToolTip", "📄 Open Mountea Inventory & Equipment Global UI Configuration\n\n❔ Define shared UI elements, like Theme, Fonts, Modals etc."),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->GlobalUIConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_GlobalConfigButton_Error", "Unable to locate the Mountea Inventory Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
							}),
							false,
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config")
						));
						
						// Inventory Config
						configSection.AddEntry(FToolMenuEntry::InitSubMenu(
							"MounteaInventory_Config_InventorySubMenu",
							LOCTEXT("MounteaSystemEditor_InventoryConfigSubMenu_Label", "Inventory"),
							LOCTEXT("MounteaSystemEditor_InventoryConfigSubMenu_ToolTip", "📄 Open Mountea Inventory configuration menus."),
							FNewToolMenuDelegate::CreateLambda([](UToolMenu* inventoryConfigSubMenu)
							{
								FToolMenuSection& inventoryConfigSection = inventoryConfigSubMenu->FindOrAddSection("MounteaInventory_Config_Inventory");
								inventoryConfigSection.Label = LOCTEXT("InvConfigInventory_Label", "Inventory");
								inventoryConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_Config",
									LOCTEXT("MounteaSystemEditor_ConfigButton_Label", "Mountea Advanced Inventory Config"),
									LOCTEXT("MounteaSystemEditor_ConfigButton_ToolTip", "📄 Open Mountea Inventory Configuration\n\n❔ Define inventory types, rarities, categories, limits and notification behaviors used by the core system."),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->AdvancedInventorySettingsConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_ConfigButton_Error", "Unable to locate the Mountea Inventory Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
								inventoryConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_UIConfig",
									LOCTEXT("MounteaSystemEditor_UIConfigButton_Label", "Mountea Advanced Inventory UI Config"),
									LOCTEXT("MounteaSystemEditor_UIConfigButton_ToolTip", "📄 Open Mountea Inventory UI Configuration\n\n❔ Define inventory UI classes, styles and themes."),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->AdvancedInventoryUISettingsConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_UIConfigButton_Error", "Unable to locate the Mountea Inventory UI Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
							}),
							false,
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config")
						));
						
						// Equipment Config
						configSection.AddEntry(FToolMenuEntry::InitSubMenu(
							"MounteaInventory_Config_EquipmentSubMenu",
							LOCTEXT("MounteaSystemEditor_EquipmentConfigSubMenu_Label", "Equipment"),
							LOCTEXT("MounteaSystemEditor_EquipmentConfigSubMenu_ToolTip", "📄 Open Mountea Equipment configuration menus."),
							FNewToolMenuDelegate::CreateLambda([](UToolMenu* equipmentConfigSubMenu)
							{
								FToolMenuSection& equipmentConfigSection = equipmentConfigSubMenu->FindOrAddSection("MounteaInventory_Config_Equipment");
								equipmentConfigSection.Label = LOCTEXT("InvConfigEquipment_Label", "Equipment");
								equipmentConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_EquipmentConfig",
									LOCTEXT("MounteaSystemEditor_EquipmentConfigButton_Label", "Mountea Advanced Equipment Config"),
									LOCTEXT("MounteaSystemEditor_EquipmentConfigButton_ToolTip", "📄 Open Mountea Equipment Configuration\n\n❔ Define Equipment types, slots, rules and other configuration."),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->AdvancedEquipmentSettingsConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_EquipmentConfigButton_Error", "Unable to locate the Mountea Equipment Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
							}),
							false,
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config")
						));
						
						// Crafting Config
						configSection.AddEntry(FToolMenuEntry::InitSubMenu(
							"MounteaInventory_Config_CraftingSubMenu",
							LOCTEXT("MounteaSystemEditor_CraftingConfigSubMenu_Label", "Crafting"),
							LOCTEXT("MounteaSystemEditor_CraftingConfigSubMenu_ToolTip", "📄 Open Mountea Crafting configuration menus."),
							FNewToolMenuDelegate::CreateLambda([](UToolMenu* equipmentConfigSubMenu)
							{
								FToolMenuSection& equipmentConfigSection = equipmentConfigSubMenu->FindOrAddSection("MounteaInventory_Config_Crafting");
								equipmentConfigSection.Label = LOCTEXT("InvConfigCrafting_Label", "Crafting");
								equipmentConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_CraftingConfig",
									LOCTEXT("MounteaSystemEditor_CraftingConfigButton_Label", "Mountea Advanced Crafting Config"),
									LOCTEXT("MounteaSystemEditor_CraftingConfigButton_ToolTip", "Open Mountea Crafting Configuration"),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->AdvancedCraftingSettingsConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_CraftingConfigButton_Error", "Unable to locate the Mountea Crafting Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
								equipmentConfigSection.AddEntry(FToolMenuEntry::InitMenuEntry(
									"MounteaInventory_CraftingUIConfig",
									LOCTEXT("MounteaSystemEditor_CraftingUIConfigButton_Label", "Mountea Advanced Crafting UI Config"),
									LOCTEXT("MounteaSystemEditor_CraftingUIConfigButton_ToolTip", "Open Mountea Crafting UI Configuration"),
									FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config"),
									FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
									{
										FText errorMessage;
										const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
										UMounteaAdvancedInventorySystemEditorStatics::OpenConfig(
											settings ? settings->AdvancedCraftingUISettingsConfig.ToSoftObjectPath() : FSoftObjectPath(),
											errorMessage,
											LOCTEXT("MounteaSystemEditor_CraftingUIConfigButton_Error", "Unable to locate the Mountea Crafting UI Config asset.\nPlease, open Inventory & Equipment Settings and select proper Config!"));
									})
								));
							}),
							false,
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Config")
						));
					}

					{
						FToolMenuSection& settingsSection = subMenu->FindOrAddSection("MounteaInventory_Settings");
						settingsSection.Label = LOCTEXT("InvSettings_Label", "Mountea Advanced Inventory Settings");
						settingsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_Settings",
							LOCTEXT("MounteaSystemEditor_SettingsButton_Label", "Mountea Advanced Inventory Settings"),
							LOCTEXT("MounteaSystemEditor_SettingsButton_ToolTip", "⚙ Open Mountea Advanced Inventory Settings\n\n❔ Configure core Advanced Inventory system settings including default behaviors, performance options, and Advanced Inventory flow parameters. Customize your Advanced Inventory system's foundation here."),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Settings"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&)
							{
								UMounteaAdvancedInventorySystemEditorStatics::OpenSettings("Project",  TEXT("Mountea Framework"), TEXT("Mountea Inventory System"));
							})
						));
						settingsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_EditorSettings",
							LOCTEXT("MounteaSystemEditor_EditorSettingsButton_Label", "Mountea Advanced Inventory Editor Settings"),
							LOCTEXT("MounteaSystemEditor_EditorSettingsButton_ToolTip", "⚙ Open Mountea Advanced Inventory Editor Settings\n\n❔ Customize your Advanced Inventory editor experience with settings for:TBD\n\nAll settings are saved in DefaultMounteaSettings.ini"),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Settings"),
							FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext&)
							{
								UMounteaAdvancedInventorySystemEditorStatics::OpenSettings("Project",  TEXT("Mountea Framework"), TEXT("Mountea Inventory System (Editor)"));
							})
						));
					}

					{
						FToolMenuSection& linksSection = subMenu->FindOrAddSection("MounteaInventory_Links");
						linksSection.Label = LOCTEXT("InvLinks_Label", "Mountea Inventory Links");
						linksSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_Support",
							LOCTEXT("MounteaSystemEditor_SupportButton_Label", "Mountea Support"),
							LOCTEXT("MounteaSystemEditor_SupportButton_ToolTip", "🆘 Open Mountea Framework Support Channel\n\n❔ Get direct assistance from our support team and community. Find solutions to common issues, share your experiences, and get help with implementation challenges. Join our active community of developers!"),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Help"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
							{
								FPlatformProcess::LaunchURL(TEXT("https://discord.gg/waYT2cn37z"), nullptr, nullptr);
							})
						));
						linksSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_Wiki",
							LOCTEXT("MounteaSystemEditor_WikiButton_Label", "Mountea Advanced Inventory Wiki"),
							LOCTEXT("MounteaSystemEditor_WikiButton_ToolTip", "📖 Open Mountea Advanced Inventory Documentation\n\n❔ Access comprehensive guides, tutorials, and API references. Find detailed examples, best practices, and advanced features to master the Mountea Advanced Inventory System. Your one-stop resource for all documentation needs."),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Wiki"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
							{
								FPlatformProcess::LaunchURL(TEXT("https://mountea.tools/docs/advancedinventoryequipmentsystem/overview/"), nullptr, nullptr);
							})
						));
						linksSection.AddEntry(FToolMenuEntry::InitMenuEntry(
							"MounteaInventory_Youtube",
							LOCTEXT("MounteaSystemEditor_YoutubeButton_Label", "Mountea Advanced Inventory Youtube"),
							LOCTEXT("MounteaSystemEditor_YoutubeButton_ToolTip", "👁️ Watch Mountea Advanced Inventory Youtube Videos\n\n❔ Visual learning resources featuring step-by-step tutorials, implementation guides, and practical examples. Perfect for both beginners and advanced users looking to expand their knowledge through video content."),
							FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Youtube"),
							FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
							{
								FPlatformProcess::LaunchURL(TEXT("https://www.youtube.com/playlist?list=PLIU53wA8zZmgc1ty7R8WDHkYg037xT503"), nullptr, nullptr);
							})
						));
					}

				}),
				false,
				FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.InventorySystemIcon")
			));
		}

		// Shared Tools section are always last; all plugins contribute; FindEntry guards prevent duplicates
		{
			FToolMenuSection& toolsSection = sharedMenu->FindOrAddSection("MounteaFramework_Tools");
			toolsSection.InsertPosition = FToolMenuInsert(NAME_None, EToolMenuInsertType::Last);
			toolsSection.Label = LOCTEXT("Tools_Label", "Mountea Tools");

			if (toolsSection.FindEntry("MounteaInventory_InventoryManager") == nullptr)
				toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
					"MounteaInventory_InventoryManager",
					LOCTEXT("MounteaSystemEditor_InventoryManagerButton_Label", "Mountea Inventory Manager"),
					LOCTEXT("MounteaSystemEditor_InventoryManagerButton_ToolTip", "💼 Open Mountea Inventory Manager Tool\n\n❔ A browser-based Inventory Template Editor for Unreal Engine projects — manage item templates online from any device. Simple, fast, and accessible.\n\n💡 Features include:\n- Create and edit item templates\n- Manage item properties and categories\n- Export templates for use in Unreal Engine projects"),
					FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.InventoryManager"),
					FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
					{
						FPlatformProcess::LaunchURL(TEXT("https://mountea-framework.github.io/InventoryManager/"), nullptr, nullptr);
					})
				));

			if (toolsSection.FindEntry("MounteaFramework_Dialoguer") == nullptr)
				toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
					"MounteaFramework_Dialoguer",
					LOCTEXT("MounteaSystemEditor_DialoguerButton_Label", "Mountea Dialoguer"),
					LOCTEXT("MounteaSystemEditor_DialoguerButton_ToolTip", "⛰ Open Mountea Dialoguer Standalone Tool\n\n❔ A powerful standalone dialogue crafting tool designed for narrative designers and writers. Create, edit, and manage complex dialogue trees with an intuitive interface. Seamlessly import your `.mnteadlg` files directly into the Mountea Dialogue System.\n\n💡 Perfect for teams wanting to separate dialogue content creation from engine implementation."),
					FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Dialoguer"),
					FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
					{
						FPlatformProcess::LaunchURL(TEXT("https://mountea.tools/dialoguer"), nullptr, nullptr);
					})
				));

			if (toolsSection.FindEntry("MounteaFramework_Launcher") == nullptr)
				toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
					"MounteaFramework_Launcher",
					LOCTEXT("MounteaSystemEditor_LauncherButton_Label", "Mountea Project Launcher"),
					LOCTEXT("MounteaSystemEditor_LauncherButton_ToolTip", "🚀 Open Mountea Project Launcher\n\n❔ A versatile standalone tool for streamlined project testing and deployment. Launch your projects with customized configurations, test different build settings, and validate implementations in various environments.\n\n💡 Features include:\n- Multiple configuration profiles\n- Quick-launch presets\n- Custom command-line parameters\n- Integrated testing tools"),
					FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Launcher"),
					FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
					{
						FPlatformProcess::LaunchURL(TEXT("https://github.com/Mountea-Framework/MounteaProjectLauncher"), nullptr, nullptr);
					})
				));

			if (toolsSection.FindEntry("MounteaFramework_Builder") == nullptr)
				toolsSection.AddEntry(FToolMenuEntry::InitMenuEntry(
					"MounteaFramework_Builder",
					LOCTEXT("MounteaSystemEditor_BuilderButton_Label", "Mountea Unreal Builder"),
					LOCTEXT("MounteaSystemEditor_BuilderButton_ToolTip", "🏗 Open Mountea Unreal Builder\n\n❔ A desktop application for automating Unreal Engine plugin and project builds across multiple engine versions and platforms."),
					FSlateIcon(FMounteaAdvancedInventoryEditorStyle::GetAppStyleSetName(), "MAISStyleSet.Builder"),
					FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext&)
					{
						FPlatformProcess::LaunchURL(TEXT("https://github.com/Mountea-Framework/MounteaUnrealBuilder"), nullptr, nullptr);
					})
				));
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaAdvancedInventorySystemEditor, MounteaAdvancedInventorySystemEditor)
