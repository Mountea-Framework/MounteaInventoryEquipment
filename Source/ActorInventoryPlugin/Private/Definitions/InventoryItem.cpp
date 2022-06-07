// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Definitions/InventoryItem.h"

void UInventoryItem::SetItemCategory(const TSubclassOf<UInventoryCategory> Category)
{
	ItemCategory = Category;
}

void UInventoryItem::SetItemRarity(const TSubclassOf<UInventoryItemRarity> Rarity)
{
	ItemRarity = Rarity;
}
