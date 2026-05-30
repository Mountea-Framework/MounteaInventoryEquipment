// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"

UMounteaAdvancedInventoryGlobalUIConfig::UMounteaAdvancedInventoryGlobalUIConfig()
{
	ValidateModalTypes();
}

const TArray<FString>& UMounteaAdvancedInventoryGlobalUIConfig::GetDefaultModalTypes()
{
	static const TArray<FString> defaultModalTypes
	{
		TEXT("Inventory.DropItem"),
		TEXT("Inventory.UseItem"),
		TEXT("Inventory.DestroyItem"),
		TEXT("Inventory.SplitStack"),
		TEXT("Crafting.NoRecipes"),
		TEXT("Crafting.CraftItem"),
		TEXT("Crafting.MissingIngredients"),
		TEXT("Equipment.EquipItem"),
		TEXT("Equipment.UnequipItem"),
		TEXT("Equipment.ReplaceItem"),
		TEXT("Pickup.PickupItem"),
		TEXT("Loot.TakeItem"),
		TEXT("Merchant.BuyItem"),
		TEXT("Merchant.SellItem"),
		TEXT("Merchant.InsufficientFunds"),
		TEXT("Stealing.StealItem")
	};
	return defaultModalTypes;
}

void UMounteaAdvancedInventoryGlobalUIConfig::ValidateModalTypes()
{
	for (const FString& defaultModalType : GetDefaultModalTypes())
	{
		if (ModalTypes.Contains(defaultModalType)) 
			continue;
		ModalTypes.Add(defaultModalType);
	}
}

TArray<FString> UMounteaAdvancedInventoryGlobalUIConfig::GetModalTypes() const
{
	TArray<FString> returnValues;
	returnValues.Reserve(ModalTypes.Num() + 1);
	returnValues.Add(TEXT("none"));
	returnValues.Append(ModalTypes.Array());
	return returnValues;
}

#if WITH_EDITOR
void UMounteaAdvancedInventoryGlobalUIConfig::SetDefaultModalTypes()
{
	ValidateModalTypes();
}

void UMounteaAdvancedInventoryGlobalUIConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UMounteaAdvancedInventoryGlobalUIConfig, ModalTypes))
		ValidateModalTypes();
}
#endif
