// All rights reserved Dominik Morse 2023

#pragma once

const FVector2D Icon12x12(12.0f, 12.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon128x128(128.f, 128.f);
const FVector2D Icon200x70(200.f, 70.f);

const FString ChangelogURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInventoryEquipment/5.0/CHANGELOG.md");
const FString SupportDiscordURL = FString("https://discord.gg/2vXWEEN");
const FString GameplayTagsURL = FString("https://raw.githubusercontent.com/Mountea-Framework/MounteaInventoryEquipment/main/Config/Tags/MounteaInventoryEquipmentTags.ini");

const FString PluginWikiPage = FString("https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki");
const FString ItemsWikiPage = FString("https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Object");
const FString ItemConfigWikiPage = FString("https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Config");
const FString ItemActionWikiPage = FString("https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Action");

const FName AdvancedMenuCategoryName("MounteaInventoryEquipment");

#define LOCTEXT_NAMESPACE "MounteaInventoryEquipmentEditorConsts"

const FText AdvancedMenuCategoryNameText = FText::FromString("Mountea Inventory & Equipment");
const FText AdvancedMenuSubCategoryName_01 = LOCTEXT("AdvancedMenuSubCategoryName_01", "1. Configuration");
const FText AdvancedMenuSubCategoryName_02 = LOCTEXT("AdvancedMenuSubCategoryName_02", "2. Data");
const FText AdvancedMenuSubCategoryName_03 = LOCTEXT("AdvancedMenuSubCategoryName_03", "3. Definitions");
const FText AdvancedMenuSubCategoryName_04 = LOCTEXT("AdvancedMenuSubCategoryName_04", "4. Components");
const FText AdvancedMenuSubCategoryName_05 = LOCTEXT("AdvancedMenuSubCategoryName_05", "5. Content Themes");

#undef LOCTEXT_NAMESPACE
