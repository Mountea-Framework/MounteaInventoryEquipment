// All rights reserved Dominik Pavlicek 2022.


#include "Widgets/InventoryItemSlotUse.h"
#include "Helpers/InventoryHelpers.h"


void UInventoryItemSlotUse::Init_Implementation(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot, const EUseType NewUseType)
{
	OriginSlot = NewOriginSlot;
	TargetSlot = NewTargetSlot;
	InventoryUseType = NewUseType;
}

void UInventoryItemSlotUse::SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget)
{
	OwningWidget = NewOwningWidget;
}

bool UInventoryItemSlotUse::IsValid_Implementation() const
{
	switch (InventoryUseType)
	{
	case EUseType::EST_Use:
	case EUseType::EST_Drop:
	case EUseType::EST_Loot:
		return OriginSlot != nullptr;
	case EUseType::EST_Merge:
	case EUseType::EST_Split:
	case EUseType::EST_Trade:
		return OriginSlot != nullptr && TargetSlot != nullptr;
	case EUseType::Default:
	default:
		return false;
	} 
}

void UInventoryItemSlotUse::SetUseType(const EUseType NewUseType)
{
	InventoryUseType = NewUseType;
}

FString UInventoryItemSlotUse::GetUseTypeText() const
{
	return GetEnumValueAsString(FString("EUseType"), InventoryUseType);
}
