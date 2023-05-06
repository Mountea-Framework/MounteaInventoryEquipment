// All rights reserved Dominik Pavlicek 2022.


#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

UMounteaInventoryEquipmentSettings::UMounteaInventoryEquipmentSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory & Equipment");
	
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
	
	ItemsWidgetCommands.Add(MounteaInventoryEquipmentConsts::MounteaInventoryWidgetCommands::ItemCommands::AddNewItem);
}

#undef LOCTEXT_NAMESPACE