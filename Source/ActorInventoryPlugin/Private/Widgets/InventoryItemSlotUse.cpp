// All rights reserved Dominik Pavlicek 2022.


#include "Widgets/InventoryItemSlotUse.h"
#include "Definitions/InventoryTypeDefinition.h"


void UInventoryItemSlotUse::Init_Implementation(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot, UInventoryTypeDefinition* NewUseType)
{
	OriginSlot = NewOriginSlot;
	TargetSlot = NewTargetSlot;
	InventoryUseType = NewUseType;
}

void UInventoryItemSlotUse::SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget)
{
	OwningWidget = NewOwningWidget;
}

void UInventoryItemSlotUse::SetUseType(UInventoryTypeDefinition* NewUseType)
{
	InventoryUseType = NewUseType;
}

FString UInventoryItemSlotUse::GetUseTypeText() const
{
	if (InventoryUseType)
	{
		return InventoryUseType->GetInventoryTypeName().ToString();
	}

	return NSLOCTEXT("Inventory Use", "Null", "Invalid").ToString();
}
