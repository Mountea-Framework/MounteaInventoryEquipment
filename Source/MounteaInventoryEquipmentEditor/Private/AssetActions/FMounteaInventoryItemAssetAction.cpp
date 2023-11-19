// All rights reserved Dominik Morse 2023.

#include "FMounteaInventoryItemAssetAction.h"

#include "ContentBrowserMenuContexts.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"


#define LOCTEXT_NAMESPACE "MounteaInventoryItemAssetAction"

FMounteaInventoryItemAssetAction::FMounteaInventoryItemAssetAction()
{
	
}

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

void FMounteaInventoryItemAssetAction::ExecuteRefreshItem(TArray<TWeakObjectPtr<UMounteaInventoryItemBase>> Items)
{
	if (Items.Num() == 1)
	{
		if (const auto Item = Items[0].Get())
		{
			Item->SetValidData();
		}
	}
	else
	{
		TArray<UMounteaInventoryItemBase*> ItemsToSync;
		for (auto ItemItr = Items.CreateConstIterator(); ItemItr; ++ItemItr)
		{
			if (const auto Item = (*ItemItr).Get())
			{
				Item->SetValidData();
			}
		}
	}
}

void FMounteaInventoryItemAssetAction::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	const auto Items = GetTypedWeakObjectPtrs<UMounteaInventoryItemBase>(InObjects);
	FAssetTypeActions_Base::GetActions(InObjects, Section);

	Section.AddMenuEntry(
		"Item_RefreshItem",
		LOCTEXT("Item_RefreshItem", "Refresh Item Data"),
		LOCTEXT("Item_RefreshItemTooltip", "Updates data for selected Item(s)."),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Refresh"),
		FUIAction(
			FExecuteAction::CreateSP( this, &FMounteaInventoryItemAssetAction::ExecuteRefreshItem, Items )
			)
		);
}

/*
const TArray<FText>& FMounteaInventoryItemAssetAction::GetSubMenus() const
{
	static const TArray<FText> AssetTypeActionSubMenu
	{
		LOCTEXT("MounteaInventoryEquipment_Sub01", "1. Inventory Item")
	};
	return AssetTypeActionSubMenu;
}
*/

void FAssetActionExtender_MounteaInventoryItem::RegisterMenus()
{
	if (!UToolMenus::IsToolMenuUIEnabled())
	{
		return;
	}
	
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");

	Section.AddDynamicEntry("MounteaInventoryItem", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
		if (!Context || Context->SelectedObjects.IsEmpty())
		{
			return;
		}
		
		for (const TWeakObjectPtr<UObject>& Object : Context->SelectedObjects)
		{
			if (UBlueprint* ItemBP = Cast<UBlueprint>(Object))
			{
				if (ItemBP->ParentClass->IsChildOf(UMounteaInventoryItemBase::StaticClass()))
				{
					const TAttribute<FText> Label = LOCTEXT("Item_RefreshItem", "Refresh Item Data");
					const TAttribute<FText> ToolTip = LOCTEXT("Item_RefreshItemTooltip", "Updates data for selected Item(s).");
					const FSlateIcon Icon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Refresh");
					const FToolMenuExecuteAction UIExecuteAction = FToolMenuExecuteAction::CreateStatic(&FAssetActionExtender_MounteaInventoryItem::ExecuteRefreshItem);

					InSection.AddMenuEntry("MounteaInventoryItemAsset_RefreshItem", Label, ToolTip, Icon, UIExecuteAction);
				}
			}
		}
	}));
}

void FAssetActionExtender_MounteaInventoryItem::ExecuteRefreshItem(const FToolMenuContext& MenuContext)
{
	UContentBrowserAssetContextMenuContext* Context = MenuContext.FindContext<UContentBrowserAssetContextMenuContext>();
	if (!Context || Context->SelectedObjects.Num() == 0)
	{
		return;
	}

	// TODO Find how to get from UBlueprint to execute its native functions
	for (const TWeakObjectPtr<UObject>& Object : Context->SelectedObjects)
	{
		UMounteaInventoryItemBase* ItemBP = Cast<UMounteaInventoryItemBase>(Object);
		if (ItemBP)
		{
			TScriptInterface<IMounteaInventoryEquipmentItem> ItemInterface = ItemBP;
			ItemInterface.SetObject(ItemBP);
			ItemInterface.SetInterface(Cast<IMounteaInventoryEquipmentItem>(ItemBP));

			ItemInterface->SetValidData();
		}
		
	}
	
}


#undef LOCTEXT_NAMESPACE
