// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryManagerComponent.h"
#include "Components/ActorInventoryComponent.h"
#include "Definitions/InventoryItem.h"

UActorInventoryManagerComponent::UActorInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorInventoryManagerComponent::AddItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer)
{
	if (OwningPlayer)
	{
		TArray<UActorComponent*> InventoryComponents = OwningPlayer->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

		if (InventoryComponents.Num())
		{
			// Only one inventory per player
			UActorComponent* InventoryComponent = InventoryComponents[0];

			if (InventoryComponent)
			{
				TScriptInterface<IActorInventoryInterface> InventoryInterface;
				InventoryInterface.SetObject(InventoryComponent);
				InventoryInterface.SetInterface(Cast<IActorInventoryInterface>(InventoryComponent));

				if (InventoryInterface)
				{
					InventoryInterface->AddItemToInventory(Item);
				}
			}
		}
	}
}

void UActorInventoryManagerComponent::AddItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer)
{
	for (UInventoryItem* Itr : Items)
	{
		AddItemToInventory(Itr, OwningPlayer);
	}
}

TArray<UInventoryItem*>  UActorInventoryManagerComponent::GetItemsFromInventory(APlayerController* OwningPlayer) const
{
	if (OwningPlayer)
	{
		TArray<UActorComponent*> InventoryComponents = OwningPlayer->GetComponentsByInterface(UActorInventoryInterface::StaticClass());

		if (InventoryComponents.Num())
		{
			// Only one inventory per player
			UActorComponent* InventoryComponent = InventoryComponents[0];

			if (InventoryComponent)
			{
				TScriptInterface<IActorInventoryInterface> InventoryInterface;
				InventoryInterface.SetObject(InventoryComponent);
				InventoryInterface.SetInterface(Cast<IActorInventoryInterface>(InventoryComponent));

				if (InventoryInterface)
				{
					return InventoryInterface->GetInventoryItems();
				}
			}
		}
	}

	return TArray<UInventoryItem*>();
}

void UActorInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UActorInventoryManagerComponent::ClearAllowedCategories()
{
	AllowedCategories.Empty();
}

void UActorInventoryManagerComponent::SetAllowedCategories(TSet<UInventoryCategory*>& Categories)
{
	ClearAllowedCategories();
	AllowedCategories = Categories;
}

void UActorInventoryManagerComponent::AddAllowedCategory(UInventoryCategory* Category)
{
	if(!ContainsAllowedCategory(Category))
	{
		AllowedCategories.Emplace(Category);
	}
}

void UActorInventoryManagerComponent::AddAllowedCategories(TSet<UInventoryCategory*>& Categories)
{
	for (const auto Itr : Categories)
	{
		AddAllowedCategory(Itr);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategory(UInventoryCategory* Category)
{
	if (ContainsAllowedCategory(Category))
	{
		AllowedCategories.Remove(Category);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedCategories(TSet<UInventoryCategory*>& Categories)
{
	for (const auto Itr : Categories)
	{
		RemoveAllowedCategory(Itr);
	}
}


void UActorInventoryManagerComponent::ClearAllowedRarities()
{
	AllowedRarities.Empty();
}

void UActorInventoryManagerComponent::SetAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities)
{
	ClearAllowedRarities();
	AllowedRarities = Rarities;
}

void UActorInventoryManagerComponent::AddAllowedRarity(UInventoryItemRarity* Rarity)
{
	if (!ContainsAllowedRarity(Rarity))
	{
		AllowedRarities.Emplace(Rarity);
	}
}

void UActorInventoryManagerComponent::AddAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities)
{
	for (const auto Itr : Rarities)
	{
		AddAllowedRarity(Itr);	
	}
}

void UActorInventoryManagerComponent::RemoveAllowedRarity(const UInventoryItemRarity* Rarity)
{
	if (ContainsAllowedRarity(Rarity))
	{
		AllowedRarities.Remove(Rarity);
	}
}

void UActorInventoryManagerComponent::RemoveAllowedRarities(const TSet<UInventoryItemRarity*>& Rarities)
{
	for (const auto Itr : Rarities)
	{
		RemoveAllowedRarity(Itr);
	}
}

void UActorInventoryManagerComponent::UpdateCategories()
{
	if (bAutoAllowParentCategories)
	{
		TSet<UInventoryCategory*> ParentCategories;
		
		for (const auto Itr : AllowedCategories)
		{
			if (Itr && Itr->GetParentCategory() && !ContainsAllowedCategory(Itr->GetParentCategory()))
			{
				ParentCategories.Add(Itr->GetParentCategory());
			}
		}

		for (const auto Itr : ParentCategories)
		{
			if (!ContainsAllowedCategory(Itr))
			{
				int32 SearchDepth = 1;
				AddParentCategory(Itr, SearchDepth);
			}
		}
	}
}

void UActorInventoryManagerComponent::AddParentCategory(UInventoryCategory* Category, int32& DepthIndex)
{
	if (DepthIndex > MaxRecursionDepth)
	{
		return;
	}
	
	if(Category->GetClass())
	{
		if (ContainsAllowedCategory(Category))
		{
			return;
		}
		else
		{
			AddAllowedCategory(Category);
			
			DepthIndex++;
			
			if (Category && Category->GetParentCategory())
			{
				AddParentCategory(Category->GetParentCategory(), DepthIndex);
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
