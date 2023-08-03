// All rights reserved Dominik Pavlicek 2023.


#include "Helpers/MounteaInventoryEquipmentBPF.h"

int UMounteaInventoryEquipmentBPF::CalculateMaxSubtractQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
	if (RequestedQuantity == 0)
	{
		return 0;
	}
	
	if (!Item)
	{
		return 0;
	}

	return 1;
}

int UMounteaInventoryEquipmentBPF::CalculateMaxAddQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
	// Return 0 if RequestedQuantity is 0 or Item is null
	if (RequestedQuantity == 0 || !Item)
	{
		return 0;
	}
    
	// If the item is not stackable, return 0 if OtherItem is not null (updating case), or 1 if it is null (adding new item case)
	if (!Item->ItemData.ItemQuantity.bIsStackable)
	{
		return OtherItem ? 0 : 1;
	}
    
	// Calculate the maximum possible quantity to be added
	const int32 CurrentQuantity = Item->ItemData.ItemQuantity.CurrentQuantity;
	const int32 MaxQuantity = Item->ItemData.ItemQuantity.MaxQuantity;
	int32 MaxPossible = MaxQuantity - CurrentQuantity;

	// If OtherItem is not null, the requested quantity should be limited by OtherItem's current quantity
	if (OtherItem)
	{
		const int32 UpdateQuantity = FMath::Min(RequestedQuantity, OtherItem->ItemData.ItemQuantity.CurrentQuantity);
		MaxPossible = FMath::Min(MaxPossible, UpdateQuantity);
	}
	else // If OtherItem is null, the requested quantity should be limited by RequestedQuantity
	{
	MaxPossible = FMath::Min(MaxPossible, RequestedQuantity);
	}

	return MaxPossible;
}



int32 UMounteaInventoryEquipmentBPF::AddItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
	if (!BaseItem) return 0;
	if (RequestedQuantity == 0) return 0;

	if (OtherItem)
	{
		
	}
	else
	{
		
	}

	return 0;
}

int32 UMounteaInventoryEquipmentBPF::RemoveItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem, const int32 RequestedQuantity)
{
	if (!BaseItem) return 0;
	if (RequestedQuantity == 0) return 0;

	if (OtherItem)
	{
		
	}
	else
	{
		
	}

	return 0;
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryEquipmentBPF::ExcludeItems(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter)
{
	if (!Filter.IsValid()) return TArray<UMounteaInventoryItemBase*>();

	TArray<UMounteaInventoryItemBase*> TempResult;

	for (const auto& Itr : ItemsToFilter)
	{
		if (!Itr) continue;

		bool bExclude = false;

		if (Filter.bSearchByTag && Itr->GetTags().HasAny(Filter.Tags))
		{
			bExclude = true;
		}
		else if (Filter.bSearchByItem && Itr == Filter.Item)
		{
			bExclude = true;
		}
		else if (Filter.bSearchByClass && Itr->IsA(Filter.Class))
		{
			bExclude = true;
		}
		else if (Filter.bSearchByGUID && Itr->GetItemGuid() == Filter.Guid)
		{
			bExclude = true;
		}

		if (!bExclude)
		{
			TempResult.Add(Itr);
		}
	}

	return TempResult;
}
