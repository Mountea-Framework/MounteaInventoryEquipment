// All rights reserved Dominik Pavlicek 2023.

#include "FMounteaInventoryItemAssetAction.h"

#include "AssetEditors/AssetEditor_MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemAssetAction"

FMounteaInventoryItemAssetAction::FMounteaInventoryItemAssetAction()
{}

FText FMounteaInventoryItemAssetAction::GetName() const
{
	return LOCTEXT("MounteaInventoryItemAssetAction_Name", "Item Definition");
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
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(AdvancedMenuCategoryName);
	}
	
	return  EAssetTypeCategories::Misc;
}

const TArray<FText>& FMounteaInventoryItemAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		LOCTEXT("MounteaInventoryEquipment_Sub01", "1. Class Definitions")
	};
	return AssetTypeActionSubMenu;
}

void FMounteaInventoryItemAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	UE_LOG(LogTemp, Error, TEXT("OpenAssetEditor"))
	
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UMounteaInventoryItemBase* Item = Cast<UMounteaInventoryItemBase>(*ObjIt))
		{
			TSharedRef<FAssetEditor_MounteaInventoryItem> NewEditor(new FAssetEditor_MounteaInventoryItem());
			NewEditor->InitAssetEditor(Mode, EditWithinLevelEditor, Item);
		}
	}
}

#undef LOCTEXT_NAMESPACE
