#include "MounteaInventoryEquipmentEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserMenuContexts.h"
#include "HttpModule.h"
#include "ToolMenuMisc.h"
#include "ToolMenus.h"
#include "AssetActions/FMounteaInventoryCategoryAssetAction.h"
#include "AssetActions/FMounteaInventoryComponentAssetAction.h"

#include "AssetActions/FMounteaInventoryItemAdditionalDataAssetAction.h"
#include "AssetActions/FMounteaInventoryItemAssetAction.h"
#include "AssetActions/FMounteaInventoryItemConfigAssetAction.h"
#include "AssetActions/FMounteaInventoryItemDescriptionsTableAssetAction.h"
#include "AssetActions/FMounteaInventoryItemsTableAssetAction.h"
#include "AssetActions/FMounteaInventoryRarityAssetAction.h"
#include "AssetActions/FMounteaInventoryThemeAssetAction.h"

#include "HelpButton/MIECommands.h"
#include "HelpButton/MIEHelpStyle.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Helpers/MIEClassStyle.h"

#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"

#include "Popups/MIEPopup.h"
#include "Popups/MIEPopupStyle.h"

#include "Serialization/JsonReader.h"
#include "Styling/SlateStyleRegistry.h"

class IMainFrameModule;
DEFINE_LOG_CATEGORY(MounteaInventoryEquipmentEditor);


#define LOCTEXT_NAMESPACE "FMounteaInventoryEquipmentEditor"

void FMounteaInventoryEquipmentEditor::StartupModule()
{
	// Try to request Changelog from GitHub
	{
		Http = &FHttpModule::Get();
		SendHTTPGet();
	}

	// Register new Category
	{
		FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(AdvancedMenuCategoryName, AdvancedMenuCategoryNameText);
	}
	
	// Asset Actions
	{
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemConfigAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryThemeAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemsTableAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemDescsAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryItemAdditionalDataAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryCategoryAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryRarityAssetAction>());
		RegisterAssetTypeAction(FAssetToolsModule::GetModule().Get(), MakeShared<FMounteaInventoryComponentAssetAction>());
	}

	// Register Styles and Commands
	{
		FMIEClassStyle::Initialize();
		FMIEClassStyle::ReloadTextures();
		
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

				RegisterClassIcons(TEXT("Resources/ClassIcons/InventoryComponentIcon"), TEXT("MounteaInventoryComponent"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemObjectIcon"), TEXT("MounteaInventoryItemBase"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemDataIcon"), TEXT("MounteaItemAdditionalData"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ItemConfigIcon"), TEXT("MounteaInventoryItemConfig"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/ThemeConfigIcon"), TEXT("MounteaInventoryThemeConfig"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/InventoryRarityIcon"), TEXT("MounteaInventoryItemRarity"));
				RegisterClassIcons(TEXT("Resources/ClassIcons/InventoryCategoryIcon"), TEXT("MounteaInventoryItemCategory"));

				//Register the created style
				FSlateStyleRegistry::RegisterSlateStyle(*InventoryEquipmentClassStyleSet.Get());
			}
		}
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
		FMIEClassStyle::Shutdown();

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

void FMounteaInventoryEquipmentEditor::OnGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

void FMounteaInventoryEquipmentEditor::SendHTTPGet()
{
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FMounteaInventoryEquipmentEditor::OnGetResponse);
	Request->SetURL(ChangelogURL);

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();
}

void FMounteaInventoryEquipmentEditor::PluginButtonClicked()
{
	const FString URL = "https://discord.gg/2vXWEEN";

	if (!URL.IsEmpty())
	{
		FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
	}
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
		FAssetActionExtender_MounteaInventoryItem::RegisterMenus();
	}
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaInventoryEquipmentEditor, MounteaInventoryEquipmentEditor)