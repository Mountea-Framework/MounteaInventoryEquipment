// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemAssetAction.h"

#include "Definitions/MounteaInventoryItem.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemAssetAction"

FMounteaInventoryItemAssetAction::FMounteaInventoryItemAssetAction()
{}

FText FMounteaInventoryItemAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemAssetAction_Name", "1. Item Definition");
}

FColor FMounteaInventoryItemAssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

UClass* FMounteaInventoryItemAssetAction::GetSupportedClass() const
{
	return UMounteaInventoryItemBase::StaticClass();
}

uint32 FMounteaInventoryItemAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("MounteaInventoryEquipment"));
	}
	
	return  EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE