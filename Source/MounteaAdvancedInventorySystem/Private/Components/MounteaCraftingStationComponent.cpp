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


#include "Components/MounteaCraftingStationComponent.h"


UMounteaCraftingStationComponent::UMounteaCraftingStationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	UMounteaCraftingStationComponent::SetComponentTickEnabled(false);
	
	bAutoActivate = true;
	
	SetIsReplicatedByDefault(true);
	SetActiveFlag(true);

	ComponentTags.Append( { TEXT("Mountea"), TEXT("Crafting") } );
}

void UMounteaCraftingStationComponent::BeginPlay()
{
	Super::BeginPlay();	
}

bool UMounteaCraftingStationComponent::IsCraftingPlaceOccupied_Implementation() const
{
	return false;
}

int32 UMounteaCraftingStationComponent::GetCraftingPlaceCapacity_Implementation() const
{
	return INDEX_NONE;
}

