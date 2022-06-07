// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Definitions/InventoryItemRarity.h"

UInventoryItemRarity::UInventoryItemRarity()
{
	SetRarityName("Common");
	SetRarityColor(FLinearColor::Gray);
	SetRarityID(0);
}

void UInventoryItemRarity::SetRarityName(const FName& Name)
{
	RarityName = Name;
}

void UInventoryItemRarity::SetRarityColor(const FLinearColor& Color)
{
	RarityColor = Color;
}

void UInventoryItemRarity::SetRarityID(const int32& ID)
{
	RarityID = ID;
}
