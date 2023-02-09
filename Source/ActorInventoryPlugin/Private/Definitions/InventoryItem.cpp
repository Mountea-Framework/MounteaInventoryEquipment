// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Definitions/InventoryItem.h"

#if WITH_EDITOR
void UInventoryItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "bIsStackable")
	{
		if (!Item.ItemQuantityData.bIsStackable)
		{
			Item.ItemQuantityData.MaxQuantity = 1;
		}
	}

	if (PropertyName == "MaxQuantity")
	{
		if (!Item.ItemQuantityData.bIsStackable)
		{
			Item.ItemQuantityData.MaxQuantity = 1;
		}
	}
}
#endif