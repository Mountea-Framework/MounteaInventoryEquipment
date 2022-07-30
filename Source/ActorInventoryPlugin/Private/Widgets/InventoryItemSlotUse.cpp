// All rights reserved Dominik Pavlicek 2022.


#include "Widgets/InventoryItemSlotUse.h"


void UInventoryItemSlotUse::Init_Implementation(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot)
{
	OriginSlot = NewOriginSlot;
	TargetSlot = NewTargetSlot;
}

void UInventoryItemSlotUse::SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget)
{
	OwningWidget = NewOwningWidget;
}

void UInventoryItemSlotUse::SetUseType(const EUseType& NewUseType)
{
	UseType = NewUseType;
}