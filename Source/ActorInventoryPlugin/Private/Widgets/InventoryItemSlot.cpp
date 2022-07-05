// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemSlot.h"

#include "Helpers/ActorInventoryPluginLog.h"

void UInventoryItemSlot::OnSlotUpdated_Implementation(const FInventorySlotData SlotData)
{
	UpdateItemSlot(SlotData);
}

bool UInventoryItemSlot::Initialize()
{
	if(Super::Initialize())
	{
		return true;
	}

	return false;
}

void UInventoryItemSlot::UpdateItemSlot(const FInventorySlotData& Data)
{
	ItemSlotData = Data;
}