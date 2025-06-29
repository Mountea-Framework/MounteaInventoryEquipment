// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools


#include "Definitions/MounteaInventoryBaseUIDataTypes.h"

#include "Blueprint/UserWidget.h"


FInventorySlot::FInventorySlot(const struct FMounteaInventoryGridSlot& GridSlot)
{
	OccupiedItemId = GridSlot.OccupiedItemId;
	SlotQuantity = GridSlot.SlotQuantity;
	SlotWidget = GridSlot.SlotWidget;
}

FMounteaInventoryGridSlot::FMounteaInventoryGridSlot() : FInventorySlot()
{
}

FMounteaInventoryGridSlot::FMounteaInventoryGridSlot(const FInventorySlot& SourceSlot)
{
	OccupiedItemId = SourceSlot.OccupiedItemId;
	SlotQuantity = SourceSlot.SlotQuantity;
	SlotWidget = SourceSlot.SlotWidget;
}

FString FMounteaInventoryGridSlot::ToString() const
{
	FString returnString = TEXT("Slot: ");
	returnString.Append(TEXT("SlotPosition: ")).Append(SlotPosition.ToString()).Append(TEXT(" | "));
	returnString.Append(TEXT("OccupiedItemId: ")).Append(OccupiedItemId.ToString()).Append(TEXT(" | "));
	returnString.Append(TEXT("SlotWidget: ")).Append( SlotWidget ? *SlotWidget->GetName() : TEXT("None") );
	return returnString;
}
