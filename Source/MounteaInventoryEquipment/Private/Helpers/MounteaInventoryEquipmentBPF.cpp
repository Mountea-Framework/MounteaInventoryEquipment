// All rights reserved Dominik Pavlicek 2022.


#include "Helpers/MounteaInventoryEquipmentBPF.h"

int UMounteaInventoryEquipmentBPF::CalculateMaxAddQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
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
		
		const int32 UpdateQuantity = RequestedQuantity != 0 ? RequestedQuantity : OtherItem->ItemData.ItemQuantity.CurrentQuantity;
	
		const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
		const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;

		const int32 MaxPossible = MaxQuantity - CurrentQuantity;
	
		const int32 PassQuantity = FMath::Min(MaxPossible, CurrentQuantity + UpdateQuantity);

		return PassQuantity;
	}
	else
	{
		// Adding new item that cannot stack mean always just and only 1
		if (!Item->ItemData.ItemQuantity.bIsStackable)
		{
			return 1;
		}

		const int32 UpdateQuantity = RequestedQuantity != 0 ? RequestedQuantity : Item->ItemData.ItemQuantity.CurrentQuantity;
	
		const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
		const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;

		const int32 MaxPossible = MaxQuantity - CurrentQuantity;
	
		const int32 PassQuantity = FMath::Min(MaxPossible, UpdateQuantity);

		return PassQuantity;
	}
	
	return 0;
}
