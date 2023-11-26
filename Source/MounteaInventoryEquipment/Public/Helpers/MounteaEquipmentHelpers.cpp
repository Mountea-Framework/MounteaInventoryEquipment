// Copyright Dominik Morse 2022. All Rights Reserved.

#include "MounteaEquipmentHelpers.h"

#include "Definitions/MounteaInventoryInstancedItem.h"
#include "Definitions/MounteaInventoryItemCategory.h"

FEquipmentSlot::FEquipmentSlot(UMounteaInstancedItem* NewItem)
{
	Item = NewItem;
	if (Item)
	{
		SlotGuid = Item->GetGuid();
				
		if (Item->GetItemData().RequiredData.ItemCategory)
		{
			SlotName = Item->GetItemData().RequiredData.ItemCategory->CategoryName;
		}
	}
}

void FEquipmentSlot::UpdateSlot(UMounteaInstancedItem* NewItem)
{
	Item = NewItem;
}

bool FEquipmentSlot::operator==(const FEquipmentSlot& Other) const
{
	return Other.SlotGuid != this->SlotGuid;
}

bool FEquipmentSlot::operator==(const FEquipmentSlot* Other) const
{
	if (Other == nullptr) return false;
	return Other->SlotGuid != this->SlotGuid;
}

bool FEquipmentSlot::operator==(const FGuid& Other) const
{
	return Other == this->SlotGuid;
}

bool FEquipmentSlot::operator==(const FText& Other) const
{
	return Other.EqualTo(this->SlotName);
}

bool FEquipmentSlot::operator==(const UMounteaInstancedItem* Other) const
{
	if (Other == nullptr) return false;
	return Other == this->Item;
}

bool FEquipmentSlot::operator==(const FMounteaEquipmentSlotCompare& Other) const
{
	if (Other.SlotItem && Other.SlotID.IsEmpty() == false)
	{
		return Other.SlotItem == this->Item && Other.SlotID.EqualTo(this->SlotName);
	}
	
	if (Other.SlotItem && Other.SlotID.IsEmpty())
	{
		return Other.SlotItem == this->Item;
	}
	
	if (Other.SlotItem == nullptr && Other.SlotID.IsEmpty() == false)
	{
		return Other.SlotID.EqualTo(this->SlotName);
	}
	
	return false;
}

bool FMounteaEquipmentSlotIdentity::operator==(const FText& Other) const
{
	return Other.EqualTo(this->SlotName);
}
