// All rights reserved Dominik Pavlicek 2022.


#include "Widgets/InventoryItemSlotSplit.h"


void UInventoryItemSlotSplit::Init_Implementation(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot)
{
	OriginSlot = NewOriginSlot;
	TargetSlot = NewTargetSlot;
}

void UInventoryItemSlotSplit::SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget)
{
	OwningWidget = NewOwningWidget;
}

void UInventoryItemSlotSplit::SetSplitType(const ESplitType& NewSplitType)
{
	SplitType = NewSplitType;
}