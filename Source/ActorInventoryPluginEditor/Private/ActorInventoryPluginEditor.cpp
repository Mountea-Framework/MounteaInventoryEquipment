#include "ActorInventoryPluginEditor.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "AssetActions/InventoryCategoryAssetActions.h"
#include "AssetActions/InventoryItemAssetActions.h"
#include "AssetActions/InventoryKeyActionAssetActions.h"
#include "AssetActions/InventoryRarityAssetActions.h"

DEFINE_LOG_CATEGORY(ActorInventoryPluginEditor);

#define LOCTEXT_NAMESPACE "FActorInventoryPluginEditor"

void FActorInventoryPluginEditor::StartupModule()
{
	UE_LOG(ActorInventoryPluginEditor, Warning, TEXT("ActorInventoryPluginEditor module has been loaded"));

	// Asset Types
	{
		// Register Category
		{
			FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("Inventory"), FText::FromString("Inventory"));
		}
	
		// Register Inventory Item Object
		{
			InventoryItemAssetActions = MakeShared<FInventoryItemAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryItemAssetActions.ToSharedRef());
		}
	
		// Register Inventory Category Data Asset
		{
			InventoryCategoryAssetActions = MakeShared<FInventoryCategoryAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryCategoryAssetActions.ToSharedRef());
		}

		// Register Inventory Rarity Data Asset
		{
			InventoryRarityAssetsActions = MakeShared<FInventoryRarityAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryRarityAssetsActions.ToSharedRef());
		}

		// Register Inventory Action Keys Data Asset
		{
			InventoryKeyActionAssetActions = MakeShared<FInventoryKeyActionAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryKeyActionAssetActions.ToSharedRef());
		}
	}
	
	// Components Thumbnails and Icons 
    {
        InventoryComponentSet = MakeShareable(new FSlateStyleSet("Inventory Component Style"));
    	InventoryManagerSet = MakeShareable(new FSlateStyleSet("Inventory Manager Style"));
		InventoryCategorySet = MakeShareable(new FSlateStyleSet("Inventory Category Style"));
		InventoryRaritySet = MakeShareable(new FSlateStyleSet("Inventory Rarity Style"));
		InventoryKeyActionsSet = MakeShareable(new FSlateStyleSet("Inventory Key Actions Style"));
		InventoryItemSet = MakeShareable(new FSlateStyleSet("Inventory Item Style"));
    
        const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("ActorInventoryPlugin");
    
        if (PluginPtr.IsValid())
        {
        	const FString ContentDir = IPluginManager::Get().FindPlugin("ActorInventoryPlugin")->GetBaseDir();
        	
        	// Inventory Component
        	{
        		InventoryComponentSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryClassThumb = new FSlateImageBrush(InventoryComponentSet->RootToContentDir(TEXT("Resources/Inventory_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryClassIcon = new FSlateImageBrush(InventoryComponentSet->RootToContentDir(TEXT("Resources/Inventory_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryClassThumb && InventoryClassIcon)
        		{
        			InventoryComponentSet->Set("ClassThumbnail.ActorInventoryComponent", InventoryClassThumb);
        			InventoryComponentSet->Set("ClassIcon.ActorInventoryComponent", InventoryClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryComponentSet.Get());
        		}
        	}

        	// Inventory Manager
	        {
        		InventoryManagerSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryManagerClassThumb = new FSlateImageBrush(InventoryManagerSet->RootToContentDir(TEXT("Resources/InventoryManager_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryManagerClassIcon = new FSlateImageBrush(InventoryManagerSet->RootToContentDir(TEXT("Resources/InventoryManager_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryManagerClassThumb && InventoryManagerClassIcon)
        		{
        			InventoryManagerSet->Set("ClassThumbnail.ActorInventoryManagerComponent", InventoryManagerClassThumb);
        			InventoryManagerSet->Set("ClassIcon.ActorInventoryManagerComponent", InventoryManagerClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryManagerSet.Get());
        		}
	        }

        	// Inventory Category
        	{
        		InventoryCategorySet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryCategoryClassThumb = new FSlateImageBrush(InventoryCategorySet->RootToContentDir(TEXT("Resources/Category_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryCategoryClassIcon = new FSlateImageBrush(InventoryCategorySet->RootToContentDir(TEXT("Resources/Category_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryCategoryClassThumb && InventoryCategoryClassIcon)
        		{
        			InventoryCategorySet->Set("ClassThumbnail.InventoryCategory", InventoryCategoryClassThumb);
        			InventoryCategorySet->Set("ClassIcon.InventoryCategory", InventoryCategoryClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryCategorySet.Get());
        		}
        	}

        	// Inventory Rarity
        	{
        		InventoryRaritySet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryRarityClassThumb = new FSlateImageBrush(InventoryRaritySet->RootToContentDir(TEXT("Resources/Rarity_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryRarityClassIcon = new FSlateImageBrush(InventoryRaritySet->RootToContentDir(TEXT("Resources/Rarity_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryRarityClassThumb && InventoryRarityClassIcon)
        		{
        			InventoryRaritySet->Set("ClassThumbnail.InventoryItemRarity", InventoryRarityClassThumb);
        			InventoryRaritySet->Set("ClassIcon.InventoryItemRarity", InventoryRarityClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryRaritySet.Get());
        		}
        	}

        	// Inventory KeyActions
        	{
        		InventoryKeyActionsSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryKeyActionsClassThumb = new FSlateImageBrush(InventoryKeyActionsSet->RootToContentDir(TEXT("Resources/KeyActions_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryKeyActionsClassIcon = new FSlateImageBrush(InventoryKeyActionsSet->RootToContentDir(TEXT("Resources/KeyActions_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryKeyActionsClassThumb && InventoryKeyActionsClassIcon)
        		{
        			InventoryKeyActionsSet->Set("ClassThumbnail.InventoryKeyAction", InventoryKeyActionsClassThumb);
        			InventoryKeyActionsSet->Set("ClassIcon.InventoryKeyAction", InventoryKeyActionsClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryKeyActionsSet.Get());
        		}
        	}

        	// Inventory Item
        	{
        		InventoryItemSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryItemClassThumb = new FSlateImageBrush(InventoryItemSet->RootToContentDir(TEXT("Resources/Item_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryItemClassIcon = new FSlateImageBrush(InventoryItemSet->RootToContentDir(TEXT("Resources/Item_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryItemClassThumb && InventoryItemClassIcon)
        		{
        			InventoryItemSet->Set("ClassThumbnail.InventoryItem", InventoryItemClassThumb);
        			InventoryItemSet->Set("ClassIcon.InventoryItem", InventoryItemClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryItemSet.Get());
        		}
        	}
        }
    }
}

void FActorInventoryPluginEditor::ShutdownModule()
{
	UE_LOG(ActorInventoryPluginEditor, Warning, TEXT("ActorInventoryPluginEditor module has been unloaded"));

	// Asset Types Cleanup
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		{
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryItemAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryCategoryAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryRarityAssetsActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryKeyActionAssetActions.ToSharedRef());
		}
	}

	// Components Thumbnails and Icons cleanup
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryComponentSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryManagerSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryCategorySet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryRaritySet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryKeyActionsSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryItemSet->GetStyleSetName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FActorInventoryPluginEditor, ActorInventoryPluginEditor)