#include "ActorInventoryPluginEditor.h"

#include "AssetActions/InventoryCategoryAssetActions.h"
#include "AssetActions/InventoryItemAssetActions.h"
#include "AssetActions/InventoryKeyActionAssetActions.h"
#include "AssetActions/InventoryRarityAssetActions.h"

DEFINE_LOG_CATEGORY(ActorInventoryPluginEditor);

#define LOCTEXT_NAMESPACE "FActorInventoryPluginEditor"

void FActorInventoryPluginEditor::StartupModule()
{
	UE_LOG(ActorInventoryPluginEditor, Warning, TEXT("ActorInventoryPluginEditor module has been loaded"));

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

void FActorInventoryPluginEditor::ShutdownModule()
{
	UE_LOG(ActorInventoryPluginEditor, Warning, TEXT("ActorInventoryPluginEditor module has been unloaded"));

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Cleanup
		{

			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryItemAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryCategoryAssetActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryRarityAssetsActions.ToSharedRef());
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryKeyActionAssetActions.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FActorInventoryPluginEditor, ActorInventoryPluginEditor)