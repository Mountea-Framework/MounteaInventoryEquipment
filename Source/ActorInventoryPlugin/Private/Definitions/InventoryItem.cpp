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

void UInventoryItem::SetItemThumbnail(UTexture2D* Thumbnail)
{
	ItemThumbnail = Thumbnail;
}

void UInventoryItem::SetItemTittle(const FText& Text)
{
	ItemTittle = Text;
}

void UInventoryItem::SetItemDescription(const FText& Text)
{
	ItemDescription = Text;
}
