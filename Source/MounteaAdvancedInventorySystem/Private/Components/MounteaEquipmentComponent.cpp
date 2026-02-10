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


#include "Components/MounteaEquipmentComponent.h"


UMounteaEquipmentComponent::UMounteaEquipmentComponent()
{
	ComponentTags.Append( { TEXT("Equipment") } );
}

bool UMounteaEquipmentComponent::EquipItem_Implementation(UObject* EquipmentItem) const
{
	return true;
}

bool UMounteaEquipmentComponent::EquipItemToSlot_Implementation(const FName& SlotId, UObject* EquipmentItem)
{
	return true;
}

bool UMounteaEquipmentComponent::UnequipItem_Implementation(UObject* EquipmentItem, bool bUseFallbackSlot)
{
	return true;
}

bool UMounteaEquipmentComponent::UnequipItemFromSlot_Implementation(const FName& SlotId, bool bUseFallbackSlot)
{
	return true;
}

bool UMounteaEquipmentComponent::IsEquipmentItemEquipped_Implementation(UObject* EquipmentItem) const
{
	return true;
}
