// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryManagerComponent.h"

UActorInventoryManagerComponent::UActorInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


