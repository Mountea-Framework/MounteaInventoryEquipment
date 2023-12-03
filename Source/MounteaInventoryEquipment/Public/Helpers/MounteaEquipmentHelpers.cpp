// Copyright Dominik Morse 2022. All Rights Reserved.

#include "MounteaEquipmentHelpers.h"

#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Definitions/MounteaInventoryItemCategory.h"

#define LOCTEXT_NAMESPACE "MounteaEquipmentSlotIdentity"

FEquipmentSlot::FEquipmentSlot(UMounteaInstancedItem* NewItem)
{
	Item = NewItem;
	if (Item)
	{
		SlotGuid = Item->GetGuid();
				
		if (Item->GetItemData().RequiredData.ItemCategory)
		{
			SlotIdentity.SlotName = Item->GetItemData().RequiredData.ItemCategory->CategoryName;
		}
	}
}

void FEquipmentSlot::UpdateSlot(UMounteaInstancedItem* NewItem)
{
	Item = NewItem;

	SlotGuid = Item != nullptr ? Item->GetGuid() : FGuid();
}

bool FEquipmentSlot::operator==(const FEquipmentSlot& Other) const
{
	return Other.SlotGuid == SlotGuid;
}

bool FEquipmentSlot::operator==(const FEquipmentSlot* Other) const
{
	if (Other == nullptr) return false;
	return Other->SlotGuid == SlotGuid;
}

bool FEquipmentSlot::operator==(const FGuid& Other) const
{
	return Other == SlotGuid;
}

bool FEquipmentSlot::operator==(const FText& Other) const
{
	return Other.EqualTo(SlotIdentity.SlotName);
}

bool FEquipmentSlot::operator==(const UMounteaInstancedItem* Other) const
{
	if (Other == nullptr) return false;
	return Other == Item;
}

bool FEquipmentSlot::operator==(const FMounteaEquipmentSlotCompare& Other) const
{
	const bool bItemSame = Other.SlotItem == Item;
	const bool bNameSame = Other.SlotID.EqualTo(SlotIdentity.SlotName);
	const bool bTagSame = Other.SlotTag == SlotIdentity.SlotTag;

	bool bEqual = true;

	if (Other.SlotItem)
	{
		if (bItemSame == false) bEqual = false;
	}

	if (Other.SlotTag.IsValid())
	{
		if (bTagSame == false) bEqual = false;
	}

	if (Other.SlotID.IsEmpty() == false)
	{
		if (bNameSame == false) bEqual = false;
	}

	return bEqual;
}

bool FMounteaEquipmentSlotIdentity::operator==(const FText& Other) const
{
	if (SlotName.IsEmpty() && Other.IsEmpty()) return true;
	return Other.ToString() == SlotName.ToString();
}

bool FMounteaEquipmentSlotIdentity::operator==(const FGameplayTag& Other) const
{
	if (SlotTag.IsValid() == false && Other.IsValid() == false) return true;
	return SlotTag == Other;
}

bool FMounteaEquipmentSlotIdentity::operator==(const FMounteaEquipmentSlotIdentity& Other) const
{
	// Check if both names are empty, then compare only tags
	if (this->SlotName.IsEmpty() && Other.SlotName.IsEmpty())
	{
		return this->SlotTag == Other.SlotTag;
	}

	// Check if names are the same (regardless of tags)
	if (this->SlotName.EqualTo(Other.SlotName))
	{
		return true;
	}

	// Check if tags are the same (regardless of names)
	if (this->SlotTag == Other.SlotTag)
	{
		return true;
	}

	// If none of the above conditions are met, the slots are not considered equal
	return false;
}

#undef LOCTEXT_NAMESPACE