// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Widgets/InventoryWidget.h"

#include "Helpers/ActorInventoryPluginLog.h"


bool UInventoryWidget::Initialize()
{
	if (Super::Initialize())
	{
		if (GetOwningPlayer())
		{
			TArray<UActorComponent*> InventoryInterfaces = GetOwningPlayer()->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

			if (InventoryInterfaces.IsValidIndex(0))
			{
				if(UActorComponent* InventoryInterface = InventoryInterfaces[0])
				{
					OwningInventory = InventoryInterface;
					OwningInventory.SetObject(InventoryInterface);
					OwningInventory.SetInterface(Cast<IActorInventoryInterface>(InventoryInterface));

					FOnInventoryUpdated& UpdateHandle = OwningInventory->GetUpdateEventHandle();
					UpdateHandle.AddUniqueDynamic(this, &UInventoryWidget::RefreshWidget);

					return true;
				}
			}
		}
	}
	
	return false;
}

void UInventoryWidget::SaveInventorySlot_Implementation(const FInventorySlotData& InventorySlot)
{
	if (OwningInventory)
	{
		OwningInventory->GetInventoryLayoutUpdateRequestHandle().Broadcast(InventorySlot);
	}
}

