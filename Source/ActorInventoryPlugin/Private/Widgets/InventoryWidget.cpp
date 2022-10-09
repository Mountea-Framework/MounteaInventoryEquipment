// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Widgets/InventoryWidget.h"

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
					UpdateHandle.AddUniqueDynamic(this, &UInventoryWidget::RefreshInventoryWidget);

					return true;
				}
			}
		}
	}
	
	return false;
}

void UInventoryWidget::InitializeInventoryWidget_Implementation(
	TScriptInterface<IActorInventoryInterface>& SourceInventory)
{
	OwningInventory = SourceInventory;
}

void UInventoryWidget::SetSelectedCategory(const FGuid& CategoryGUID)
{
	SelectedCategory = CategoryGUID;

	OnCategorySelected(CategoryGUID);
}

void UInventoryWidget::OnCategorySelected_Implementation(const FGuid CategoryGUID)
{
	OnCategoryUpdated.Broadcast(SelectedCategory);
}

void UInventoryWidget::SaveInventorySlot_Implementation(const FInventorySlotData& InventorySlot)
{
	if (OwningInventory)
	{
		OwningInventory->GetInventoryLayoutUpdateRequestHandle().Broadcast(InventorySlot);
	}
}

