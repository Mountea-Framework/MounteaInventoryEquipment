// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorInventoryPlugin.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FActorInventoryPluginModule"

void FActorInventoryPluginModule::StartupModule()
{
	/*
	// Components Thumbnails and Icons 
    {
        InventorySet = MakeShareable(new FSlateStyleSet("Inventory Style"));
    	InventoryManagerSet = MakeShareable(new FSlateStyleSet("Inventory Manager Style"));
    
        const TSharedPtr<IPlugin> PluginPtr = IPluginManager::Get().FindPlugin("ActorInventoryPlugin");
    
        if (PluginPtr.IsValid())
        {
        	const FString ContentDir = IPluginManager::Get().FindPlugin("ActorInventoryPlugin")->GetBaseDir();
        	
        	// Inventory
        	{
        		InventorySet->SetContentRoot(ContentDir);
        		
        		FSlateImageBrush* InventoryClassThumb = new FSlateImageBrush(InventorySet->RootToContentDir(TEXT("Resources/Inventory_128"), TEXT(".png")), FVector2D(128.f, 128.f));
        		FSlateImageBrush* InventoryClassIcon = new FSlateImageBrush(InventorySet->RootToContentDir(TEXT("Resources/Inventory_16"), TEXT(".png")), FVector2D(16.f, 16.f));
        		if (InventoryClassThumb && InventoryClassIcon)
        		{
        			InventorySet->Set("ClassThumbnail.ActorInventoryComponent", InventoryClassThumb);
        			InventorySet->Set("ClassIcon.ActorInventoryComponent", InventoryClassIcon);
     
        			//Register the created style
        			FSlateStyleRegistry::RegisterSlateStyle(*InventorySet.Get());
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
        }
    }
    */
}

void FActorInventoryPluginModule::ShutdownModule()
{
	/*
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(InventorySet->GetStyleSetName());
		FSlateStyleRegistry::UnRegisterSlateStyle(InventoryManagerSet->GetStyleSetName());
	}
	*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActorInventoryPluginModule, ActorInventoryPlugin)