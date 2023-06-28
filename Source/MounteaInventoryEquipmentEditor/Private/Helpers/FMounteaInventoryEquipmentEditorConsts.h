// All rights reserved Dominik Pavlicek 2023

#pragma once

const FVector2D Icon12x12(12.0f, 12.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon128x128(128.f, 128.f);
const FVector2D Icon200x70(200.f, 70.f);

const FString ChangelogURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInventoryEquipment/5.0/CHANGELOG.md");

const FName AdvancedMenuCategoryName("MounteaInventoryEquipment");

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentEditorConsts"

const FText AdvancedMenuCategoryNameText = FText::FromString("Mountea Inventory & Equipment");
const FText AdvancedMenuSubCategoryName_01 = LOCTEXT("AdvancedMenuSubCategoryName_01", "1. Configuration");
const FText AdvancedMenuSubCategoryName_02 = LOCTEXT("AdvancedMenuSubCategoryName_02", "2. Data");

#undef LOCTEXT_NAMESPACE
