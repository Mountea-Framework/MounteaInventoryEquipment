#include "MounteaInventoryEquipmentEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserMenuContexts.h"
#include "GameplayTagsManager.h"
#include "HttpModule.h"
#include "ToolMenuMisc.h"
#include "ToolMenus.h"

#include "AssetActions/FMounteaInventoryCategoryAssetAction.h"
#include "AssetActions/FMounteaInventoryComponentAssetAction.h"
#include "AssetActions/FMounteaInventoryContentThemeAssetAction.h"
#include "AssetActions/FMounteaInventoryItemActionAssetAction.h"
#include "AssetActions/FMounteaInventoryItemBlueprintAssetAction.h"
#include "AssetActions/FMounteaInventoryItemConfigAssetAction.h"
#include "AssetActions/FMounteaInventoryRarityAssetAction.h"
#include "AssetActions/FMounteaInventoryThemeAssetAction.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaItemAction.h"

#include "DetailsPanel/MounteaItemConfig_Details.h"
#include "DetailsPanel/MounteaItemConfig_DetailsPanel.h"
#include "DetailsPanel/MounteaSettings_Details.h"

#include "HelpButton/MIECommands.h"
#include "HelpButton/MIEHelpStyle.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Helpers/MIEEditorStyle.h"

#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"

#include "Popups/MIEPopup.h"
#include "Popups/MIEPopupStyle.h"

#include "Serialization/JsonReader.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Settings/MounteaInventoryEquipmentSettingsEditor.h"
#include "Setup/MounteaInventoryItemConfig.h"
#include "Styling/SlateStyleRegistry.h"

class IMainFrameModule;

DEFINE_LOG_CATEGORY(MounteaInventoryEquipmentEditor);

#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentEditor"

void FMounteaInventoryEquipmentEditor::StartupModule()
{
	// Try to request Changelog from GitHub
	{
		Http = &FHttpModule::Get();
		SendHTTPGet_Changelog();
		SendHTTPGet_Tags();
	}

	// Register new Category
	{
		FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(AdvancedMenuCategoryName, AdvancedMenuCategoryNameText);
	}
	
	// Asset Actions
	{
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemConfigAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryThemeAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryCategoryAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryRarityAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryComponentAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemActionAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryContentThemeAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryContentThemeAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemBlueprintAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FCategoryThemeAssetActions>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FTextThemeAssetActions>());
	}

	// Register Styles and Commands
	{
		FMIEEditorStyle::Initialize();
		FMIEEditorStyle::ReloadTextures();
		
		FMIEHelpStyle::Initialize();
		FMIEHelpStyle::ReloadTextures();

		FMIEPopupStyle::Initialize();

		FMIECommands::Register();

		PluginCommands = MakeShareable(new FUICommandList);

		PluginCommands->MapAction
		(
			FMIECommands::Get().PluginAction,
			FExecuteAction::CreateRaw(this, &FMounteaInventoryEquipmentEditor::PluginButtonClicked), 
			FCanExecuteAction()
		);

		IMainFrameModule& mainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
		mainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMounteaInventoryEquipmentEditor::RegisterMenus));

		// Register Class Icons
		{
			InventoryEquipmentClassStyleSet = MakeShareable(new FSlateStyleSet("InventoryEquipmentClassStyleSet"));
			const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("MounteaInventoryEquipment");

			if (PluginPtr.IsValid())
			{
				const FString ContentDir = IPluginManager::Get().FindPlugin("MounteaInventoryEquipment")->GetBaseDir();

				InventoryEquipmentClassStyleSet->SetContentRoot(ContentDir);

				RegisterClassIcons(TEXT("Resources/ClassIcons/InventoryComponentIcon"),	TEXT("MounteaInventoryComponent"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemObjectIcon"),			TEXT("MounteaInventoryItemBase"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemDataIcon"),			TEXT("MounteaItemAdditionalData"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemConfigIcon"),			TEXT("MounteaInventoryItemConfig"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ThemeConfigIcon"),		TEXT("MounteaInventoryThemeConfig"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/InventoryRarityIcon"),	TEXT("MounteaInventoryItemRarity"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/InventoryCategoryIcon"),	TEXT("MounteaInventoryItemCategory"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemActionIcon"),			TEXT("MounteaInventoryItemAction"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/EquipmentComponentIcon"),	TEXT("MounteaEquipmentComponent"));

				//Register the created style
				FSlateStyleRegistry::RegisterSlateStyle(*InventoryEquipmentClassStyleSet.Get());
			}
		}
	}

	//Register custom Details
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		{
			TArray<FOnGetDetailCustomizationInstance> CustomClassLayouts =
			{
				FOnGetDetailCustomizationInstance::CreateStatic(&FMounteaItemConfig_Details::MakeInstance),
				FOnGetDetailCustomizationInstance::CreateStatic(&FMounteaSettings_Details::MakeInstance)
			};
			RegisteredCustomClassLayouts =
			{
				UMounteaInventoryItemConfig::StaticClass()->GetFName(),
				UMounteaInventoryEquipmentSettings::StaticClass()->GetFName()
			};
			for (int32 i = 0; i < RegisteredCustomClassLayouts.Num(); i++)
			{
				PropertyModule.RegisterCustomClassLayout(RegisteredCustomClassLayouts[i], CustomClassLayouts[i]);
			}
		}

		{
			TArray<FOnGetPropertyTypeCustomizationInstance> CustomPropertyTypeLayouts =
		   {
				FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMounteaItemConfig_DetailsPanel::MakeInstance),
			};
			RegisteredCustomPropertyTypeLayout =
			{
				FMounteaItemConfig::StaticStruct()->GetFName()
			};
			for (int32 i = 0; i < RegisteredCustomPropertyTypeLayout.Num(); i++)
			{
				PropertyModule.RegisterCustomPropertyTypeLayout(RegisteredCustomPropertyTypeLayout[i], CustomPropertyTypeLayouts[i]);
			}
		}

		PropertyModule.NotifyCustomizationModuleChanged();
	}
	
	UE_LOG(MounteaInventoryEquipmentEditor, Warning, TEXT("MounteaInventoryEquipmentEditor module has been loaded"));
}

void FMounteaInventoryEquipmentEditor::RegisterClassIcons(const FString& Path, const FString& ClassName)
{
	{
		FSlateImageBrush* NewClassThumbBrush = new FSlateImageBrush(InventoryEquipmentClassStyleSet->RootToContentDir(Path, TEXT(".png")), FVector2D(512.f, 512.f));
		FSlateImageBrush* NewClassIconBrush = new FSlateImageBrush(InventoryEquipmentClassStyleSet->RootToContentDir(Path, TEXT(".png")), FVector2D(16.f, 16.f));
		if (NewClassThumbBrush && NewClassIconBrush)
		{
			FString ThumbnailName = FString("ClassThumbnail.");
			ThumbnailName.Append(ClassName);

			FString IconName = FString("ClassIcon.");
			IconName.Append(ClassName);
			
			InventoryEquipmentClassStyleSet->Set(FName(ThumbnailName), NewClassThumbBrush);
			InventoryEquipmentClassStyleSet->Set(FName(IconName), NewClassIconBrush);
		}
	}
}

void FMounteaInventoryEquipmentEditor::ShutdownModule()
{
	// Asset Types Cleanup
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			{
				for (auto& Itr : CreatedAssetTypeActions)
				{
					FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(Itr.ToSharedRef());
				}
			}
		}
	}

	// Help Button Cleanup
	{
		UToolMenus::UnRegisterStartupCallback(this);

		UToolMenus::UnregisterOwner(this);

		FMIEHelpStyle::Shutdown();

		FMIECommands::Unregister();
	}

	// Style Cleanup
	{
		FMIEPopupStyle::Shutdown();
	}

	// Asset Types Cleanup
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			for (auto const& Itr : CreatedAssetTypeActions)
			{
				FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(Itr.ToSharedRef());
			}
		}
	}

	// Unregister Class Styles
	{
		FMIEEditorStyle::Shutdown();

		//Register the created style
		FSlateStyleRegistry::UnRegisterSlateStyle(*InventoryEquipmentClassStyleSet.Get());
	}
	
	UE_LOG(MounteaInventoryEquipmentEditor, Warning, TEXT("MounteaInventoryEquipmentEditor module has been unloaded"));
}

void FMounteaInventoryEquipmentEditor::RegisterAssetTypeAction(IAssetTools& AssetTools, const TSharedRef<IAssetTypeActions>& Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

void FMounteaInventoryEquipmentEditor::OnGetResponse_Changelog(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ResponseBody;
	if (Response.Get() == nullptr) return;
	
	if (Response.IsValid() && Response->GetResponseCode() == 200)
	{
		ResponseBody = Response->GetContentAsString();
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	}

	// Register Popup even if we have no response, this way we can show at least something
	{
		MIEPopup::Register(ResponseBody);
	}
}

void FMounteaInventoryEquipmentEditor::OnGetResponse_Tags(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ResponseBody;
	if (Response.Get() == nullptr) return;
	
	if (Response.IsValid() && Response->GetResponseCode() == 200)
	{
		ResponseBody = Response->GetContentAsString();
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	}
	
	if (!DoesHaveValidTags())
	{
		CreateTagsConfig(ResponseBody);
	}
	else
	{
		UpdateTagsConfig(ResponseBody);
	}
}

void FMounteaInventoryEquipmentEditor::SendHTTPGet_Changelog()
{
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FMounteaInventoryEquipmentEditor::OnGetResponse_Changelog);
	Request->SetURL(ChangelogURL);

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

void FMounteaInventoryEquipmentEditor::SendHTTPGet_Tags()
{
	if (DoesHaveValidTags())
	{
		const UMounteaInventoryEquipmentSettingsEditor* Settings = GetDefault<UMounteaInventoryEquipmentSettingsEditor>();
		if (!Settings->AllowCheckTagUpdate())
		{
			return;
		}
	}
	
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FMounteaInventoryEquipmentEditor::OnGetResponse_Tags);
	Request->SetURL(GameplayTagsURL);

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

bool FMounteaInventoryEquipmentEditor::DoesHaveValidTags() const
{
	if (!GConfig) return false;
	
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInventoryEquipmentTags.ini";

	if (FPaths::FileExists(ConfigFilePath))
	{
		FString ConfigContent;
		FConfigFile* ConfigFile = GConfig->Find(ConfigFilePath);

		return true;
	}
	else
	{
		return false;
	}
}

void FMounteaInventoryEquipmentEditor::RefreshGameplayTags()
{
	TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"));
	check(ThisPlugin.IsValid());
	
	UGameplayTagsManager::Get().EditorRefreshGameplayTagTree();
}

void FMounteaInventoryEquipmentEditor::UpdateTagsConfig(const FString& NewContent)
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInventoryEquipmentTags.ini";

	FConfigFile* CurrentConfig = GConfig->Find(ConfigFilePath);

	FString CurrentContent;
	CurrentConfig->WriteToString(CurrentContent);

	TArray<FString> Lines;
	NewContent.ParseIntoArray(Lines, TEXT("\n"), true);

	TArray<FString> CleanedLines;
	for (FString& Itr : Lines)
	{
		if (Itr.Equals("[/Script/GameplayTags.GameplayTagsList]")) continue;

		if (Itr.Contains("GameplayTagList="))
		{
			FString NewValue = Itr.Replace(TEXT("GameplayTagList="), TEXT(""));

			CleanedLines.Add(NewValue);
		}
	}

	if (!CurrentContent.Equals(NewContent))
	{
		TArray<FString> CurrentLines;
		FConfigFile NewConfig;
		NewConfig.SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CleanedLines);
		CurrentConfig->GetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CurrentLines);

		for (const FString& Itr : CleanedLines)
		{
			if (CurrentLines.Contains(Itr)) continue;

			CurrentLines.AddUnique(Itr);
		}

		CurrentConfig->SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CurrentLines);
		CurrentConfig->Write(ConfigFilePath);

		RefreshGameplayTags();
	}
}

void FMounteaInventoryEquipmentEditor::CreateTagsConfig(const FString& NewContent)
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInventoryEquipmentTags.ini";

	TArray<FString> Lines;
	NewContent.ParseIntoArray(Lines, TEXT("\n"), true);

	TArray<FString> CleanedLines;
	for (FString& Itr : Lines)
	{
		if (Itr.Equals("[/Script/GameplayTags.GameplayTagsList]")) continue;

		if (Itr.Contains("GameplayTagList="))
		{
			FString NewValue = Itr.Replace(TEXT("GameplayTagList="), TEXT(""));

			CleanedLines.Add(NewValue);
		}
	}
	
	FConfigFile NewConfig;
	NewConfig.SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CleanedLines);
	NewConfig.Write(ConfigFilePath);
}

void FMounteaInventoryEquipmentEditor::PluginButtonClicked()
{
	FPlatformProcess::LaunchURL(*SupportDiscordURL, nullptr, nullptr);
}

void FMounteaInventoryEquipmentEditor::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	// Register in Window tab
	{
		if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help"))
		{
			if (Menu->ContainsSection("MounteaFramework") == false)
			{
				FToolMenuSection& Section = Menu->FindOrAddSection("Mountea Framework");
				
				Section.InsertPosition.Position = EToolMenuInsertType::First;
				Section.Label = FText::FromString(TEXT("Mountea Framework"));
				
				FToolMenuEntry SupportEntry = Section.AddMenuEntryWithCommandList
				(
					FMIECommands::Get().PluginAction,
					PluginCommands,
					NSLOCTEXT("MounteaSupport", "TabTitle", "Mountea Support"),
					NSLOCTEXT("MounteaSupport", "TooltipText", "Opens Mountea Framework Support channel"),
					FSlateIcon(FMIEHelpStyle::GetStyleSetName(), "MIEHelpStyleSet.Toolbar.HelpIcon.small")
				);
				SupportEntry.Name = FName("MounteaFrameworkSupport");
			}
		}
	}

	// Register in Level Editor Toolbar
	{
		if (UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar"))
		{
			if (ToolbarMenu->ContainsSection("MounteaFramework") == false)
			{
				FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Mountea Framework");
				Section.Label = FText::FromString(TEXT("Mountea Framework"));
			
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMIECommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			
				Entry.InsertPosition.Position = EToolMenuInsertType::First;
				Entry.Icon = FSlateIcon(FMIEHelpStyle::GetStyleSetName(), "MIEHelpStyleSet.Toolbar.HelpIcon");
				Entry.Name = FName("MounteaFrameworkSupport");
			}
		}

	}

	// Item Menus
	{
		
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInventoryEquipmentEditor, MounteaInventoryEquipmentEditor)