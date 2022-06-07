// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryComponent.h"

UActorInventoryComponent::UActorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UActorInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

