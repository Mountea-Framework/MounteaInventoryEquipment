// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemSlot.h"

#include "Helpers/ActorInventoryPluginLog.h"

bool UInventoryItemSlot::Initialize()
{
	if(Super::Initialize())
	{
		return true;
	}

	return false;
}

void UInventoryItemSlot::SlotUpdated_Implementation(const FInventorySlotData& Data)
{
	AInvP_LOG(Warning, TEXT("SlotUpdated"))
	ItemSlotData = Data;
}
