// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Widgets/InventoryWidget.h"


bool UInventoryWidget::Initialize()
{
	bool bResult = Super::Initialize();

	if (!bResult)
	{
		return bResult;
	}

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
			}
		}
	}

	return bResult;
}
