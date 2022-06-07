// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Definitions/InventoryItemRarity.h"

UInventoryItemRarity::UInventoryItemRarity()
{
	SetRarityName("Common");
	SetRarityColor(FLinearColor::Gray);
}

void UInventoryItemRarity::SetRarityName(const FName& Name)
{
	RarityName = Name;
}

void UInventoryItemRarity::SetRarityColor(const FLinearColor& Color)
{
	RarityColor = Color;
}
