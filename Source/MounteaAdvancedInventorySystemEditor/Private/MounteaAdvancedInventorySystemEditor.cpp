// All rights reserved Dominik Morse 2024

#include "MounteaAdvancedInventorySystemEditor.h"

#include "AssetToolsModule.h"
#include "AssetActions/MounteaAdvancedInventorySettingsConfig_AssetAction.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMounteaAdvancedInventorySystemEditor"

void FMounteaAdvancedInventorySystemEditor::StartupModule()
{
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
			
			// Dialogue specific icons
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

				//Register the created style
				FSlateStyleRegistry::RegisterSlateStyle(*AdvancedInventorySet.Get());
			}
		}
	}

	// Asset Actions
	{
		AssetActions.Add(MakeShared<FMounteaAdvancedInventorySettingsConfig_AssetAction>());

		for (const auto& Itr : AssetActions)
		{
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(Itr.ToSharedRef());
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
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaAdvancedInventorySystemEditor, MounteaAdvancedInventorySystemEditor)