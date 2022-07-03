// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Definitions/InventoryItemRarity.h"

void UInventoryItemRarity::SetRarityName(const FText& Name)
{
	InventoryRarityData.RarityName = Name;
}

void UInventoryItemRarity::SetRarityColor(const FLinearColor& Color)
{
	InventoryRarityData.RarityColor = Color;
}
