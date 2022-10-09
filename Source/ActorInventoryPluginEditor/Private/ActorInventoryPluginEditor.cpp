#include "ActorInventoryPluginEditor.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"

#include "AssetActions/InventoryCategoryAssetActions.h"
#include "AssetActions/InventoryItemAssetActions.h"
#include "AssetActions/InventoryKeyActionAssetActions.h"
#include "AssetActions/InventoryRarityAssetActions.h"
#include "AssetActions/InventoryTransactionAssetActions.h"
#include "AssetActions/InventoryTypeDefinitionAssetActions.h"

#include "Kismet2/KismetEditorUtilities.h"

#include "Utilities/ActorInventoryEditorUtilities.h"

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

		// Register Inventory Type Definition Data Asset
		{
			InventoryTypeDefinitionAssetActions = MakeShared<FInventoryTypeDefinitionAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryTypeDefinitionAssetActions.ToSharedRef());
		}

		// Register Inventory Type Definition Data Asset
		{
			InventoryTransactionAssetActions = MakeShared<FInventoryTransactionAssetActions>();
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryTransactionAssetActions.ToSharedRef());
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
		InventoryItemComponentSet = MakeShareable(new FSlateStyleSet("Inventory Item Component Style"));
		InventoryTypeDefinitionSet = MakeShareable(new FSlateStyleSet("Inventory Type Definition Style"));
		InventoryTransactionDefinitionSet = MakeShareable(new FSlateStyleSet("Inventory Transaction Set"));
    
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

        	// Inventory Item Component
        	{
        		InventoryItemComponentSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryItemComponentClassThumb = new FSlateImageBrush(InventoryItemComponentSet->RootToContentDir(TEXT("Resources/InventoryItemComponent_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryItemComponentClassIcon = new FSlateImageBrush(InventoryItemComponentSet->RootToContentDir(TEXT("Resources/InventoryItemComponent_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryItemComponentClassThumb && InventoryItemComponentClassIcon)
        		{
        			InventoryItemComponentSet->Set("ClassThumbnail.ActorInventoryItemComponent", InventoryItemComponentClassThumb);
        			InventoryItemComponentSet->Set("ClassIcon.ActorInventoryItemComponent", InventoryItemComponentClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryItemComponentSet.Get());
        		}
        	}

        	// Inventory Type Definition
        	{
        		InventoryTypeDefinitionSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryTypeDefinitionClassThumb = new FSlateImageBrush(InventoryTypeDefinitionSet->RootToContentDir(TEXT("Resources/InventoryType_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryTypeDefinitionClassIcon = new FSlateImageBrush(InventoryTypeDefinitionSet->RootToContentDir(TEXT("Resources/InventoryType_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryTypeDefinitionClassThumb && InventoryTypeDefinitionClassIcon)
        		{
        			InventoryTypeDefinitionSet->Set("ClassThumbnail.InventoryTypeDefinition", InventoryTypeDefinitionClassThumb);
        			InventoryTypeDefinitionSet->Set("ClassIcon.InventoryTypeDefinition", InventoryTypeDefinitionClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryTypeDefinitionSet.Get());
        		}
        	}

        	// Inventory Transaction Definition
        	{
        		InventoryTransactionDefinitionSet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryTransactionClassThumb = new FSlateImageBrush(InventoryTransactionDefinitionSet->RootToContentDir(TEXT("Resources/InventoryTransaction_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryTransactionClassIcon = new FSlateImageBrush(InventoryTransactionDefinitionSet->RootToContentDir(TEXT("Resources/InventoryTransaction_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryTransactionClassThumb && InventoryTransactionClassIcon)
        		{
        			InventoryTransactionDefinitionSet->Set("ClassThumbnail.InventoryTransaction", InventoryTransactionClassThumb);
        			InventoryTransactionDefinitionSet->Set("ClassIcon.InventoryTransaction", InventoryTransactionClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventoryTransactionDefinitionSet.Get());
        		}
        	}
        }
    }

	// Register Pre-made Events and Functions
	{
		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UInventoryKeyAction::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FActorInventoryPluginEditor::HandleNewCustomKeyActionBlueprintCreated)
		);

		FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback
		(
			this,
			UInventoryTransaction::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FActorInventoryPluginEditor::HandleNewInventoryTransactionBlueprintCreated)
		);
		
		/* This will create default disabled events like Tick or BeginPlay
		FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent
		(
			this,
			UInventoryTransaction::StaticClass(),
			TEXT("ProcessTransaction")
		);
		*/
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
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryTypeDefinitionAssetActions.ToSharedRef());
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
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryItemComponentSet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryTypeDefinitionSet->GetStyleSetName());
	}
}

void FActorInventoryPluginEditor::HandleNewCustomKeyActionBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FActorInventoryEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UInventoryKeyAction, ExecuteAction),
		UInventoryKeyAction::StaticClass()
	);
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}
}

void FActorInventoryPluginEditor::HandleNewInventoryTransactionBlueprintCreated(UBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UEdGraph* FunctionGraph = FActorInventoryEditorUtilities::BlueprintGetOrAddFunction
	(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UInventoryTransaction, ProcessTransaction),
		UInventoryTransaction::StaticClass()
	);
	if (FunctionGraph)
	{
		Blueprint->LastEditedDocuments.Add(FunctionGraph);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FActorInventoryPluginEditor, ActorInventoryPluginEditor)