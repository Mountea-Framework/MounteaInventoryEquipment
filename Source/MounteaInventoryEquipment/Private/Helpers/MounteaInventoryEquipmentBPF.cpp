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

/* TODO: Verify the new version is working
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
*/

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

TArray<UMounteaInventoryItemBase*> UMounteaInventoryEquipmentBPF::ExcludeItems(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter)
{
	return ExcludeItems_Impl(Filter, ItemsToFilter, true);
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryEquipmentBPF::ExcludeItems_Impl(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter, const bool& bIsFirstCall)
{
	if (!Filter.IsValid()) return TArray<UMounteaInventoryItemBase*>();

	TArray<UMounteaInventoryItemBase*> TempResult;
	if (Filter.bSearchByTag)
	{
		for (const auto& Itr : ItemsToFilter)
		{
			if (Itr && !Itr->GetTags().HasAny(Filter.Tags))
			{
				TempResult.Add(Itr);
			}
		}
	}
	
	if (Filter.bSearchByItem)
	{
		for (const auto& Itr : ItemsToFilter)
		{
			if (Itr && Itr != Filter.Item)
			{
				TempResult.Add(Itr);
			}
		}
	}

	if (Filter.bSearchByClass)
	{
		for (const auto& Itr : ItemsToFilter)
		{
			if (Itr && !Itr->IsA(Filter.Class))
			{
				TempResult.Add(Itr);
			}
		}
	}
	
	if (Filter.bSearchByGUID)
	{
		for (const auto& Itr : ItemsToFilter)
		{
			if (Itr && Itr->GetItemGuid() != Filter.Guid)
			{
				TempResult.Add(Itr);
			}
		}
	}

	// If this is the first call, make a recursive call with bIsFirstCall set to false
	if (bIsFirstCall)
	{
		return ExcludeItems_Impl(Filter, TempResult, false);
	}
	else  // If this is not the first call, don't make a recursive call
	{
		return TempResult;
	}
}

