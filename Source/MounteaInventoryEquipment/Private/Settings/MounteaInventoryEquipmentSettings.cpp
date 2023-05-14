// All rights reserved Dominik Pavlicek 2022.


#include "Settings/MounteaInventoryEquipmentSettings.h"

#include "Helpers/MounteaInventoryHelpers.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

UMounteaInventoryEquipmentSettings::UMounteaInventoryEquipmentSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory & Equipment");
	
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::RefreshInventoryWidget);

	InventoryUpdateMessages.Add(EInventoryUpdateResult::EIC_Success, LOCTEXT("InventoryUpdateMessages_Success", "Inventory Updated"));
	InventoryUpdateMessages.Add(EInventoryUpdateResult::EIC_Failed,  LOCTEXT("InventoryUpdateMessages_Failed", "Inventory Update Failed"));
	
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem);
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::UpdateItem);
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::RemoveItem);

	ItemUpdateMessages.Add(EItemUpdateResult::EIC_Success_AddItem, LOCTEXT("ItemUpdateMessages_SuccessAdd", "Success - Add Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIC_Success_UpdateItem, LOCTEXT("ItemUpdateMessages_SuccessUpdate", "Success - Update Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIC_Success_RemovedItem,  LOCTEXT("InventoryUpdateMessages_RemovedItem", "Success - Removed Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIC_Success_SomeAdd,  LOCTEXT("InventoryUpdateMessages_SomeAddItem", "Success - Partially Added"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIC_Failed_InvalidItem,  LOCTEXT("InventoryUpdateMessages_InvalidItem", "Failed - Invalid Item"));
	ItemUpdateMessages.Add(EItemUpdateResult::EIC_Failed_LimitReached,  LOCTEXT("InventoryUpdateMessages_InvalidItem", "Failed - Max Quantity"));
}

#undef LOCTEXT_NAMESPACE