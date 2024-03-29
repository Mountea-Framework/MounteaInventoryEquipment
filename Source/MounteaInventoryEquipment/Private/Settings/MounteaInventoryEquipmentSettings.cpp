// All rights reserved Dominik Pavlicek 2023.


#include "Settings/MounteaInventoryEquipmentSettings.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "Setup/MounteaInventoryConfig.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

UMounteaInventoryEquipmentSettings::UMounteaInventoryEquipmentSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory & Equipment");
	
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::ShowInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshItemsWidgets);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RemoveItemWidget);

	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::InitializeEquipmentWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::CreateEquipmentWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::ShowEquipmentWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::HideEquipmentWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshEquipmentWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::RefreshItemsWidgets);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::UnequipItemWidget);
	EquipmentWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaEquipmentWidgetCommands::EquipmentCommands::EquipItemWidget);

	InventoryUpdateMessages.Add(EInventoryUpdateResult::EIUR_Success, LOCTEXT("InventoryUpdateMessages_Success", "Inventory Updated"));
	InventoryUpdateMessages.Add(EInventoryUpdateResult::EIUR_Failed,  LOCTEXT("InventoryUpdateMessages_Failed", "Inventory Update Failed"));
	
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem);
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::UpdateItem);
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::RemoveItem);

	ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_AddItem, LOCTEXT("ItemUpdateMessages_SuccessAdd", "Success - Add Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_UpdateItem, LOCTEXT("ItemUpdateMessages_SuccessUpdate", "Success - Update Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_RemovedItem,  LOCTEXT("InventoryUpdateMessages_RemovedItem", "Success - Removed Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Success_SomeAdd,  LOCTEXT("InventoryUpdateMessages_SomeAddItem", "Success - Partially Added"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Failed_InvalidItem,  LOCTEXT("InventoryUpdateMessages_InvalidItem", "Failed - Invalid Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIUR_Failed_LimitReached,  LOCTEXT("InventoryUpdateMessages_InvalidItem", "Failed - Max Quantity"));

	ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::CleanupTooltip);
	ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::ShowTooltip);
	ItemTooltipWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemTooltipCommands::HideTooltip);

	InventoryUpdateData.Add
	(
		EInventoryUpdateResult::EIUR_Failed,
		FInventoryNotificationData(nullptr, LOCTEXT("InventoryNotificationData_Failed", "Inventory Update Failed"), 3.f, FLinearColor(FColor::Red), 1)
	);
	InventoryUpdateData.Add
	(
		EInventoryUpdateResult::EIUR_Success,
		FInventoryNotificationData(nullptr, LOCTEXT("InventoryNotificationData_Success", "Inventory Updated"), 1.5f, FLinearColor(FColor::White), 0)
	);

	MinDisplayWeight = 1;

	bUIDebug = false;
	bDragDropAllowed = true;

	DefaultInventoryConfigClass = UMounteaInventoryConfig::StaticClass();
	DefaultItemClass = UMounteaInventoryItemBase::StaticClass();
	DefaultItemConfigClass = UMounteaInventoryItemConfig::StaticClass();
}

#undef LOCTEXT_NAMESPACE