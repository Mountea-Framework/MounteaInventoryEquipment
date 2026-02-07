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


#include "Components/MounteaEquipmentItemComponent.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"

UMounteaEquipmentItemComponent::UMounteaEquipmentItemComponent() : 
	bAutoActivates(1),
	bRequiresActivationEvent(0),
	EquipmentItemState(EEquipmentItemState::EES_Idle)
{
}

// TODO: Handle server and client re
bool UMounteaEquipmentItemComponent::SetEquipmentItemState_Implementation(const EEquipmentItemState NewEquipmentItemState)
{
	if (NewEquipmentItemState != EquipmentItemState)
	{
		EquipmentItemState = NewEquipmentItemState;
		OnEquipmentItemStateChanged.Broadcast(this, NewEquipmentItemState);
		return true;
	}
	
	return false;
}

bool UMounteaEquipmentItemComponent::SetAutoActive_Implementation(const bool bValue)
{
	if (bValue != bAutoActivates)
	{
		bAutoActivates = bValue;
		return true;
	}
	
	return false;
}

bool UMounteaEquipmentItemComponent::SetRequiresActivationEvent_Implementation(const bool bValue)
{
	if (bValue != bRequiresActivationEvent)
	{
		bRequiresActivationEvent = bValue;
		return true;
	}
	return false;
}
