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


#include "Components/MounteaAdvancedInventoryLoadoutComponent.h"

#include "Definitions/MounteaAdvancedInventoryLoadout.h"

UMounteaAdvancedInventoryLoadoutComponent::UMounteaAdvancedInventoryLoadoutComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaAdvancedInventoryLoadoutComponent::SetComponentTickEnabled(false);

	bAutoActivate = true;

	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append({ TEXT("Mountea"), TEXT("Loadout") });
}

bool UMounteaAdvancedInventoryLoadoutComponent::LoadLoadout_Implementation()
{
	// TODO: 
	// 1. Get all items
	// 2. Load them all to inventory
	// 3. From the Inventory get all
	// 4. Attach those that are relevant to Equipment
	return true;
}

TArray<UMounteaAdvancedInventoryLoadoutItem*> UMounteaAdvancedInventoryLoadoutComponent::GetLoadoutItems_Implementation() const
{
	if (!IsValid(LoadoutConfiguration))
		return {};
	return LoadoutConfiguration->Items;
}

void UMounteaAdvancedInventoryLoadoutComponent::Server_LoadLoadout_Implementation()
{
	Execute_LoadLoadout(this);
}

void UMounteaAdvancedInventoryLoadoutComponent::BeginPlay()
{
	Super::BeginPlay();	
}

