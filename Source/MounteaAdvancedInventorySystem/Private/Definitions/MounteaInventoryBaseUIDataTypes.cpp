// All rights reserved Dominik Morse 2024


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
