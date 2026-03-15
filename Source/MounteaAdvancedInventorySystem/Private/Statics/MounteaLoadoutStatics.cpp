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


#include "Statics/MounteaLoadoutStatics.h"

#include "Interfaces/Loadouts/MounteaAdvancedInventoryLoadoutsInterface.h"

bool UMounteaLoadoutStatics::IsValidLoadout(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaAdvancedInventoryLoadoutsInterface>();
}

bool UMounteaLoadoutStatics::LoadLoadout(UObject* Target)
{
	return IsValidLoadout(Target) ? IMounteaAdvancedInventoryLoadoutsInterface::Execute_LoadLoadout(Target) : false;
}

UMounteaAdvancedInventoryLoadoutConfig* UMounteaLoadoutStatics::GetLoadout(UObject* Target)
{
	return IsValidLoadout(Target) ? IMounteaAdvancedInventoryLoadoutsInterface::Execute_GetLoadout(Target) : nullptr;
}

TArray<UMounteaAdvancedInventoryLoadoutItem*> UMounteaLoadoutStatics::GetLoadoutItems(UObject* Target)
{
	return IsValidLoadout(Target) ? IMounteaAdvancedInventoryLoadoutsInterface::Execute_GetLoadoutItems(Target) : TArray<UMounteaAdvancedInventoryLoadoutItem*>();
}

bool UMounteaLoadoutStatics::ShouldLoadAutomatically(UObject* Target)
{
	return IsValidLoadout(Target) ? IMounteaAdvancedInventoryLoadoutsInterface::Execute_ShouldLoadAutomatically(Target) : false;
}

void UMounteaLoadoutStatics::SetAutomaticLoad(UObject* Target, const bool bNewValue)
{
	if (IsValidLoadout(Target))
		IMounteaAdvancedInventoryLoadoutsInterface::Execute_SetAutomaticLoad(Target, bNewValue);
}
