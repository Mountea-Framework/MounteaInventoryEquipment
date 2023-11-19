// All rights reserved Dominik Morse 2023.


#include "Settings/MounteaInventoryEquipmentSettingsEditor.h"

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentSettingsEditor"

UMounteaInventoryEquipmentSettingsEditor::UMounteaInventoryEquipmentSettingsEditor()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Inventory & Equipment (Editor)");
	
	bCheckForTagsUpdate = false;
}

#undef LOCTEXT_NAMESPACE