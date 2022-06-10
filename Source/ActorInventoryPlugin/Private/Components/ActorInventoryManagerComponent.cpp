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


void UActorInventoryManagerComponent::ClearAllowedCategories()
{
	AllowedCategories.Empty();
}

void UActorInventoryManagerComponent::SetAllowedCategories(const TSet<TSubclassOf<UInventoryCategory>>& Categories)
{
	ClearAllowedCategories();
	AllowedCategories = Categories;
}

void UActorInventoryManagerComponent::AddAllowedCategory(const TSubclassOf<UInventoryCategory>& Category)
{
	if(!ContainsAllowedCategory(Category))
	{
		AllowedCategories.Add(Category);
	}
}

void UActorInventoryManagerComponent::AddAllowedCategories(const TSet<TSubclassOf<UInventoryCategory>>& Categories)
{
	for (const TSubclassOf<UInventoryCategory> Itr : Categories)
	{
		AddAllowedCategory(Itr);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategory(const TSubclassOf<UInventoryCategory>& Category)
{
	if (ContainsAllowedCategory(Category))
	{
		AllowedCategories.Remove(Category);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategories(const TSet<TSubclassOf<UInventoryCategory>>& Categories)
{
	for (const TSubclassOf<UInventoryCategory> Itr : Categories)
	{
		RemoveAllowedCategory(Itr);
	}
}


void UActorInventoryManagerComponent::ClearAllowedRarities()
{
	AllowedRarities.Empty();
}

void UActorInventoryManagerComponent::SetAllowedRarities(const TSet<TSubclassOf<UInventoryItemRarity>>& Rarities)
{
	ClearAllowedRarities();
	AllowedRarities = Rarities;
}

void UActorInventoryManagerComponent::AddAllowedRarity(const TSubclassOf<UInventoryItemRarity>& Rarity)
{
	if (!ContainsAllowedRarity(Rarity))
	{
		AllowedRarities.Add(Rarity);
	}
}

void UActorInventoryManagerComponent::AddAllowedRarities(const TSet<TSubclassOf<UInventoryItemRarity>>& Rarities)
{
	for (const TSubclassOf<UInventoryItemRarity>& Itr : Rarities)
	{
		AddAllowedRarity(Itr);	
	}
}

void UActorInventoryManagerComponent::RemoveAllowedRarity(const TSubclassOf<UInventoryItemRarity>& Rarity)
{
	if (ContainsAllowedRarity(Rarity))
	{
		AllowedRarities.Remove(Rarity);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedRarities(const TSet<TSubclassOf<UInventoryItemRarity>>& Rarities)
{
	for (const TSubclassOf<UInventoryItemRarity> Itr : Rarities)
	{
		RemoveAllowedRarity(Itr);
	}
}

void UActorInventoryManagerComponent::UpdateCategories()
{
	if (bAutoAllowParentCategories)
	{
		TSet<TSubclassOf<UInventoryCategory>> ParentCategories;
		
		for (const TSubclassOf<UInventoryCategory>& Itr : AllowedCategories)
		{
			if (Itr.GetDefaultObject() && Itr.GetDefaultObject()->GetParentCategory() && !ContainsAllowedCategory(Itr.GetDefaultObject()->GetParentCategory()))
			{
				ParentCategories.Add(Itr.GetDefaultObject()->GetParentCategory());
			}
		}

		for (const TSubclassOf<UInventoryCategory>& Itr : ParentCategories)
		{
			if (!ContainsAllowedCategory(Itr))
			{
				AddParentCategory(Itr);
			}
		}
	}
}

void UActorInventoryManagerComponent::AddParentCategory(const TSubclassOf<UInventoryCategory>& Category)
{
	if(Category->GetClass())
	{
		if (ContainsAllowedCategory(Category))
		{
			AInvP_LOG(Warning, TEXT("Category Already there!"))
			return;
		}
		else
		{
			AInvP_LOG(Warning, TEXT("Adding new Category!"))
			AddAllowedCategory(Category);
			if (Category.GetDefaultObject() && Category.GetDefaultObject()->GetParentCategory())
			{
				AddParentCategory(Category.GetDefaultObject()->GetParentCategory());
			}
		}
	}
}

void UActorInventoryManagerComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "AllowedCategories")
	{
		UpdateCategories();
	}
};
