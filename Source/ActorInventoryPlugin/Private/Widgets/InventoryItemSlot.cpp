// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemSlot.h"

#include "Helpers/ActorInventoryPluginLog.h"
#include "Widgets/InventoryWidget.h"

void UInventoryItemSlot::OnSlotUpdated_Implementation(const FInventorySlotData& SlotData)
{
	UpdateItemSlot(SlotData);

	if (OwningInventoryWidget)
	{
		OwningInventoryWidget->SaveInventorySlot(SlotData);
	}
}

void UInventoryItemSlot::OnSlotInspected_Implementation()
{
	// TODO: Validation
}

bool UInventoryItemSlot::Initialize()
{
	if(Super::Initialize())
	{
		return true;
	}

	return false;
}

void UInventoryItemSlot::UpdateItemSlot(const FInventorySlotData& SlotData)
{
	ItemSlotData = SlotData;
}
