// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "MounteaItemHelpers.h"

#include "Definitions/MounteaInventoryInstancedItem.h"

bool FItemSlotStack::operator==(const FItemSlotStack& Other) const
{
	return Other.StackGuid == StackGuid;
}

bool FItemSlot::AreStacksValid() const
{
	if (Item)
	{
		const FGuid ItemGuid = Item->GetGuid();
		for (const FItemSlotStack& Stack : Stacks)
		{
			if (Stack.StackGuid != ItemGuid)
				return false;
		}
	}
	return true;
}

bool FItemSlot::IsStackSizeValid() const
{
	int32 TotalSize = 0;
	for (const FItemSlotStack& Stack : Stacks)
	{
		TotalSize += Stack.StackSize;
		if (TotalSize > Item->GetQuantity())
			return false;
	}
	return true;
}

bool FItemSlot::operator==(const FItemSlot& Other) const
{
	return Other.Item == Item;
}

bool FItemSlot::operator==(const FGuid& Other) const
{
	if (Item)
	{
		return Item->GetGuid() == Other;
	}

	return false;
}

bool FItemSlot::operator==(const UMounteaInstancedItem* Other) const
{
	return Item == Other;
}
