// All rights reserved Dominik Pavlicek 2022.


#include "Widgets/InventoryItemSlotUse.h"
#include "Helpers/InventoryHelpers.h"
#include "Widgets/InventoryItemSlot.h"
#include "Widgets/InventoryWidget.h"


void UInventoryItemSlotUse::Init_Implementation(UInventoryItemSlot* NewOriginSlot, UInventoryItemSlot* NewTargetSlot, const EUseType NewUseType)
{
	OriginSlot = NewOriginSlot;
	TargetSlot = NewTargetSlot;
	InventoryUseType = NewUseType;

	if (OriginSlot && OriginSlot->GetOwningInventoryWidget())
	{
		OriginSlotOwningInventory = OriginSlot->GetOwningInventoryWidget()->GetOwningInventory();
	}
	else
	{
		InventoryUseType = EUseType::EST_Invalid;
	}
	
	if (TargetSlot && TargetSlot->GetOwningInventoryWidget())
	{
		TargetSlotOwningInventory = TargetSlot->GetOwningInventoryWidget()->GetOwningInventory();
	}
}

void UInventoryItemSlotUse::SetOwningInventoryWidget(UInventoryWidget* NewOwningWidget)
{
	OwningWidget = NewOwningWidget;
}

bool UInventoryItemSlotUse::IsValid_Implementation() const
{
	if (OriginSlot)
	{
		if (!OriginSlot->DoesHaveItem())
		{
			return false;
		}
		
		if (OriginSlotOwningInventory.GetObject() == nullptr)
		{
			return false;
		}

		if (TargetSlotOwningInventory.GetObject() == nullptr)
		{
			return false;
		}
		
		return true;
	}
	else
	{
		return false;
	}
	
	/*
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
	*/
}

void UInventoryItemSlotUse::SetUseType(const EUseType NewUseType)
{
	InventoryUseType = NewUseType;
}

FString UInventoryItemSlotUse::GetUseTypeText() const
{
	return GetEnumValueAsString(FString("EUseType"), InventoryUseType);
}

void UInventoryItemSlotUse::SetTransactionClass(const TSubclassOf<UInventoryTransaction> NewTransactionClass)
{
	TransactionClass = NewTransactionClass;
}
