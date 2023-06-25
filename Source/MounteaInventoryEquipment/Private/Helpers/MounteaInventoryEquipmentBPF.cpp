// All rights reserved Dominik Pavlicek 2023.


#include "Helpers/MounteaInventoryEquipmentBPF.h"

int UMounteaInventoryEquipmentBPF::CalculateMaxAddQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
	if (RequestedQuantity == 0)
	{
		return 0;
	}
	
	if (!Item)
	{
		return 0;
	}
	
	if (OtherItem)
	{
		// Updating item that cannot stack mean always just and only 0
		if (!Item->ItemData.ItemQuantity.bIsStackable)
		{
			return 0;
		}
		
		const int32 UpdateQuantity = FMath::Min(RequestedQuantity, OtherItem->ItemData.ItemQuantity.CurrentQuantity);
	
		const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
		const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;

		int32 MaxPossible = MaxQuantity - CurrentQuantity;
		MaxPossible = FMath::Min(MaxPossible, UpdateQuantity);

		return MaxPossible;
	}
	else
	{
		// Adding new item that cannot stack mean always just and only 1
		if (!Item->ItemData.ItemQuantity.bIsStackable)
		{
			return 1;
		}
		
		const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
		const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;

		int32 MaxPossible = MaxQuantity - CurrentQuantity;
		MaxPossible = FMath::Min(MaxPossible, RequestedQuantity);

		return MaxPossible;
	}
	
	return 0;
}

bool UMounteaInventoryEquipmentBPF::AddItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
	if (!BaseItem) return false;
	if (RequestedQuantity == 0) return false;

	if (OtherItem)
	{
		
	}
	else
	{
		
	}

	return false;
}
