// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "MounteaItemHelpers.h"

#include "Definitions/MounteaInventoryInstancedItem.h"

bool FItemSlotStack::Serialize(FArchive& Ar)
{
	// Call the appropriate serialization function for the 'int32' type.
	Ar << StackSize;

	// Serialize the 'FGuid' types.
	Ar << StackGuid;
	Ar << SlotGuid;

	return true;
}

bool FItemSlotStack::operator==(const FItemSlotStack& Other) const
{
	return Other.StackGuid == this->StackGuid;
}

bool FItemSlotStack::operator==(const FGuid& Other) const
{
	return this->StackGuid == Other;
}

FItemSlot::FItemSlot(UMounteaInstancedItem* NewItem)
{
	Item = NewItem;
	if (Item)
	{
		SlotGuid = Item->GetGuid();
	}
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
	return Other.Item == this->Item;
}

bool FItemSlot::operator==(const FItemSlot* Other) const
{
	if (Other)
	{
		return this == Other;
	}
	return false;
}

bool FItemSlot::operator==(const FGuid& Other) const
{
	if (Item)
	{
		return this->Item->GetGuid() == Other;
	}
	return false;
}

bool FItemSlot::operator==(const UMounteaInstancedItem* Other) const
{
	return this->Item == Other;
}

bool FItemSlot::Serialize(FArchive& Ar)
{
	// Serialize the Item.
	Ar << Item;
	
	// Serialize the Stacks set.
	Ar << Stacks;

	return true;
}

FArchive& operator<<(FArchive& Ar, FItemSlotStack& Element)
{
	// Serialize each member of the struct
	Ar << Element.StackSize;
	Ar << Element.StackGuid;
	Ar << Element.SlotGuid;

	// Return the FArchive reference to allow for chaining << calls.
	return Ar;
}

FArchive& operator<<(FArchive& Ar, FItemSlot& Element)
{
	// Serialize each member of the struct
	Ar << Element.Item;
	Ar << Element.Stacks;

	// Return the FArchive reference to allow for chaining << calls.
	return Ar;
}