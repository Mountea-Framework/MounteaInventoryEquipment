// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "MounteaInventoryTestFixtures.h"
#include "GameplayTagsManager.h"
#include "Definitions/MounteaAdvancedAttachmentSlot.h"
#include "Definitions/MounteaAdvancedAttachmentSlotBase.h"
#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Statics/MounteaEquipmentStatics.h"

// Not covered: EquipItem / UnequipItem / occupied-slot behaviour. A valid equip request needs
// ItemTemplate->SpawnActor to be an actor class whose CDO carries a UMounteaEquipmentItemComponent
// (UMounteaEquipmentStatics::IsTargetClassValid); no native class does, and a test-only UCLASS or
// Blueprint asset is out of scope here.

using MounteaInventoryTest::FInventoryWorldFixture;

#define MOUNTEA_EQUIPMENT_TEST(ClassName, Name) \
	IMPLEMENT_SIMPLE_AUTOMATION_TEST(ClassName, "Mountea.InventorySystem.Equipment." Name, \
		EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

MOUNTEA_EQUIPMENT_TEST(FMounteaEquipmentSlotTypeCompatibilityTest, "SlotTypeCompatibility")
bool FMounteaEquipmentSlotTypeCompatibilityTest::RunTest(const FString& Parameters)
{
	const FGameplayTag quickUse = UGameplayTagsManager::Get().RequestGameplayTag(
		TEXT("Mountea_Inventory.Equipment.ItemType.QuickUse"), false);
	const FGameplayTag itemTypeParent = UGameplayTagsManager::Get().RequestGameplayTag(
		TEXT("Mountea_Inventory.Equipment.ItemType"), false);
	if (!TestTrue(TEXT("Plugin gameplay tags are registered"), quickUse.IsValid() && itemTypeParent.IsValid()))
	{
		return false;
	}

	UMounteaInventoryItemTemplate* itemTemplate = FInventoryWorldFixture::MakeTemplate(1);
	itemTemplate->EquipmentItemType = quickUse;
	const FMounteaInventoryItem item = FInventoryWorldFixture::MakeItem(itemTemplate, 1);

	const UMounteaAdvancedEquipmentSettingsConfig* config = NewObject<UMounteaAdvancedEquipmentSettingsConfig>(GetTransientPackage());
	UMounteaAdvancedAttachmentSlot* slot = NewObject<UMounteaAdvancedAttachmentSlot>(GetTransientPackage());
	slot->SlotName = TEXT("TestSlot");

	// Slot tag container is expanded with parents, so the exact item tag is accepted.
	slot->AllowedItemTypes = FGameplayTagContainer(quickUse);
	TestTrue(TEXT("Matching item type is accepted"), UMounteaEquipmentStatics::ValidateSlotItemTypeCompatibility(item, slot, config));

	// Slot only allows the parent tag; the child item type is not an exact match.
	slot->AllowedItemTypes = FGameplayTagContainer(itemTypeParent);
	TestFalse(TEXT("Non-matching item type is rejected"), UMounteaEquipmentStatics::ValidateSlotItemTypeCompatibility(item, slot, config));

	return true;
}

#endif
