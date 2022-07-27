#include "ActorInventoryPluginEditor.h"

#include "AssetActions/InventoryCategoryAssetActions.h"

DEFINE_LOG_CATEGORY(ActorInventoryPluginEditor);

#define LOCTEXT_NAMESPACE "FActorInventoryPluginEditor"

FActorInventoryPluginEditor::FActorInventoryPluginEditor()
{}

void FActorInventoryPluginEditor::StartupModule()
{
	UE_LOG(ActorInventoryPluginEditor, Warning, TEXT("ActorInventoryPluginEditor module has been loaded"));
	
	FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("Inventory"), FText::FromString("Inventory"));

	InventoryCategoryAssetActions = MakeShared<FInventoryCategoryAssetActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(InventoryCategoryAssetActions.ToSharedRef());
}

void FActorInventoryPluginEditor::ShutdownModule()
{
	UE_LOG(ActorInventoryPluginEditor, Warning, TEXT("ActorInventoryPluginEditor module has been unloaded"));

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(InventoryCategoryAssetActions.ToSharedRef());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FActorInventoryPluginEditor, ActorInventoryPluginEditor)