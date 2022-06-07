// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryManagerComponent.h"

UActorInventoryManagerComponent::UActorInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorInventoryManagerComponent::ClearAllowedCategories()
{
	AllowedCategories.Empty();
}

void UActorInventoryManagerComponent::SetAllowedCategories(const TArray<TSubclassOf<UInventoryCategory>> Categories)
{
	AllowedCategories.Empty();
	AllowedCategories = Categories;
}

void UActorInventoryManagerComponent::AddAllowedCategory(const TSubclassOf<UInventoryCategory> Category)
{
	if(!AllowedCategories.Contains(Category))
	{
		AllowedCategories.Add(Category);
	}
}

void UActorInventoryManagerComponent::AddAllowedCategories(const TArray<TSubclassOf<UInventoryCategory>> Categories)
{
	for (const TSubclassOf<UInventoryCategory> Itr : Categories)
	{
		AddAllowedCategory(Itr);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategory(const TSubclassOf<UInventoryCategory> Category)
{
	if (AllowedCategories.Contains(Category))
	{
		AllowedCategories.Remove(Category);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategories(const TArray<TSubclassOf<UInventoryCategory>> Categories)
{
	for (const TSubclassOf<UInventoryCategory> Itr : Categories)
	{
		RemoveAllowedCategory(Itr);
	}
}

void UActorInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
