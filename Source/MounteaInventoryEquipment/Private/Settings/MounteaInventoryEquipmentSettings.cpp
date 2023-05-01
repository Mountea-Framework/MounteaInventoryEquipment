// All rights reserved Dominik Pavlicek 2022.


#include "Settings/MounteaInventoryEquipmentSettings.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettings"

UMounteaInventoryEquipmentSettings::UMounteaInventoryEquipmentSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory & Equipment");
	
	InventoryWidgetCommands.Add(MounteaInventoryWidgetCommands::InventoryCommands::InitializeInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryWidgetCommands::InventoryCommands::CreateInventoryWidget);
	InventoryWidgetCommands.Add(MounteaInventoryWidgetCommands::InventoryCommands::HideInventoryWidget);
}

#undef LOCTEXT_NAMESPACE