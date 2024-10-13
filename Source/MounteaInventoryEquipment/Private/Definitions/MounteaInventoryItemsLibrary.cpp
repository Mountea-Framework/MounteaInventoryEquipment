﻿// All rights reserved Dominik Morse (Pavlicek) 2024

#include "Definitions/MounteaInventoryItemsLibrary.h"

#include "Definitions/MounteaInventoryItemBlueprint.h"

FMounteaInventoryItemsLibraryRow::FMounteaInventoryItemsLibraryRow(UMounteaInventoryItemBlueprint* NewItem, const int32 Quantity) 	: InitialQuantity(Quantity), InventoryItemBlueprint(NewItem)
{
	UpdateDisplayName();
}

void FMounteaInventoryItemsLibraryRow::UpdateDisplayName()
{
	InventoryItemBlueprintName = InventoryItemBlueprint ? InventoryItemBlueprint->GetPrimaryData().ItemName.ToString() : TEXT("none");
}

bool FMounteaInventoryItemsLibraryRow::IsRowValid() const
{
	return InventoryItemBlueprint.IsValid();
}

bool FMounteaInventoryItemsLibraryRow::operator==(const FGuid& ItemGuid) const
{
	auto storedItem = InventoryItemBlueprint.LoadSynchronous();
	if (!IsValid(storedItem))
		return false;

	return storedItem->GetBlueprintGuid() == ItemGuid;
}

bool FMounteaInventoryItemsLibraryRow::operator==(const UMounteaInventoryItemBlueprint* ItemBlueprint) const
{
	if (!IsValid(ItemBlueprint) || !IsValid(InventoryItemBlueprint.LoadSynchronous()))
		return false;

	if (ItemBlueprint == InventoryItemBlueprint)
		return true;
	
	return ItemBlueprint->GetBlueprintGuid() == InventoryItemBlueprint->GetBlueprintGuid();
}

bool FMounteaInventoryItemsLibraryRow::operator==(const FMounteaInventoryItemsLibraryRow& Other) const
{
	if (!IsValid(Other.InventoryItemBlueprint.LoadSynchronous()) || !IsValid(InventoryItemBlueprint.LoadSynchronous()))
		return false;

	if (InventoryItemBlueprint == Other.InventoryItemBlueprint)
		return true;
	
	return InventoryItemBlueprint->GetBlueprintGuid() == Other.InventoryItemBlueprint->GetBlueprintGuid();
}

bool FMounteaInventoryItemsLibraryRow::operator!=(const FGuid& ItemGuid) const
{
	return !(*this==ItemGuid);
}

bool FMounteaInventoryItemsLibraryRow::operator!=(const UMounteaInventoryItemBlueprint* ItemBlueprint) const
{
	return !(*this == ItemBlueprint);
}

bool FMounteaInventoryItemsLibraryRow::operator!=(const FMounteaInventoryItemsLibraryRow& Other) const
{
	return !(*this == Other);
}


TArray<UMounteaInventoryItemBlueprint*> UMounteaInventoryItemsLibrary::GetInventoryItemBlueprints() const
{
	TArray<UMounteaInventoryItemBlueprint*> result;

	for (const auto& Itr : InventoryItemBlueprints)
		result.Add(Itr.InventoryItemBlueprint.LoadSynchronous());

	return result;
}

UMounteaInventoryItemBlueprint* UMounteaInventoryItemsLibrary::GetInventoryItemByGuid(const FGuid& ItemGuid, bool& bItemFound) const
{
	for (const FMounteaInventoryItemsLibraryRow& Row : InventoryItemBlueprints)
	{
		auto rowItem = Row.InventoryItemBlueprint.LoadSynchronous();
		if (!rowItem) continue;
		
		if (Row == ItemGuid)
		{
			bItemFound = true;
			return rowItem;
		}
	}
	
	bItemFound = false;
	return nullptr;
}

UMounteaInventoryItemBlueprint* UMounteaInventoryItemsLibrary::GetInventoryItemByBlueprint(UMounteaInventoryItemBlueprint* ItemBlueprint, bool& bItemFound) const
{
	for (const FMounteaInventoryItemsLibraryRow& Row : InventoryItemBlueprints)
	{
		auto rowItem = Row.InventoryItemBlueprint.LoadSynchronous();
		if (!rowItem) continue;
		
		if (Row == ItemBlueprint)
		{
			bItemFound = true;
			return rowItem;
		}
	}
	
	bItemFound = false;
	return nullptr;
}

uint32 GetTypeHash(const FMounteaInventoryItemsLibraryRow& Row)
{
	uint32 Hash = GetTypeHash(Row.InitialQuantity);

	if (IsValid(Row.InventoryItemBlueprint.LoadSynchronous()))
	{
		Hash = HashCombine(Hash, GetTypeHash(Row.InventoryItemBlueprint->GetBlueprintGuid()));
	}

	return Hash;
}

#if WITH_EDITOR

void UMounteaInventoryItemsLibrary::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemsLibrary, InventoryItemBlueprints))
	{
		for (auto& Itr : InventoryItemBlueprints)
			Itr.UpdateDisplayName();
	}

	if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode() && PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue())
	{
		if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryItemsLibrary, InventoryItemBlueprints))
		{
			for (auto& Itr : InventoryItemBlueprints)
				Itr.UpdateDisplayName();
		}
	}
}

#endif


