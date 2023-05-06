// All rights reserved Dominik Pavlicek 2022.

#include "FMounteaInventoryItemConfigAssetAction.h"

#include "Setup/MounteaInventoryItemConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryItemConfigAssetAction"

FMounteaInventoryItemConfigAssetAction::FMounteaInventoryItemConfigAssetAction()
{}

FText FMounteaInventoryItemConfigAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemConfigAssetAction_Name", "?. Item Config");
}

FColor FMounteaInventoryItemConfigAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FMounteaInventoryItemConfigAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemConfig::StaticClass();
}

uint32 FMounteaInventoryItemConfigAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Mountea Inventory & Equipment"));
	}
	
	return  EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE