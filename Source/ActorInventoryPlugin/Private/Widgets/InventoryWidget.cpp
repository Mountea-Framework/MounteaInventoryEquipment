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

			if (InventoryInterfaces.Num() > 0)
			{
				if(UActorComponent* InventoryInterface = InventoryInterfaces[0])
				{
					OwningInventory = InventoryInterface;
					OwningInventory.SetObject(InventoryInterface);
					OwningInventory.SetInterface(Cast<IActorInventoryInterface>(InventoryInterface));

					FOnInventoryUpdated& UpdateHandle = OwningInventory->GetUpdateEventHandle();
					UpdateHandle.AddUniqueDynamic(this, &UInventoryWidget::RefreshWidget);

					FOnInventoryLayoutSaveRequested& LayoutSaveRequestedHandle = OwningInventory->GetInventoryLayoutUpdateRequestHandle();
					LayoutSaveRequestedHandle.AddUniqueDynamic(this, &UInventoryWidget::SaveItemLayout);

					return true;
				}
			}
		}
	}
	
	return false;
}

void UInventoryWidget::SaveItemLayout_Implementation(const FIntPoint& SlotCoordinates, class UInventoryItemSlot* SlotWidget)
{
	AInvP_LOG(Warning, TEXT("SaveItemLayout"))
	if (OwningInventory)
	{
		OwningInventory->GetInventoryLayoutUpdateRequestHandle().Broadcast(SlotCoordinates, SlotWidget);
	}
}
