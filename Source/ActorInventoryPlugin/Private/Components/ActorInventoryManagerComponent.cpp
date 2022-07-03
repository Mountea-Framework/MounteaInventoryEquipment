// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryManagerComponent.h"
#include "Definitions/InventoryItem.h"
#include "Widgets/InventoryWidget.h"

UActorInventoryManagerComponent::UActorInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bAllowDragDrop = true;
	bAllowPackSplit = true;
	bAllowAllCategoriesCategory = true;
}


bool UActorInventoryManagerComponent::AddItemToInventory(UInventoryItem* Item, APlayerController* OwningPlayer) const
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
					UInventoryItem* NewItem = NewObject<UInventoryItem>();
					NewItem->SetItem(Item->GetItem());
					
					return InventoryInterface->AddItemToInventory(Item);
				}
			}
		}
	}

	return false;
}

bool UActorInventoryManagerComponent::AddItemsToInventory(TArray<UInventoryItem*> Items, APlayerController* OwningPlayer) const
{
	for (UInventoryItem* Itr : Items)
	{
		if (!AddItemToInventory(Itr, OwningPlayer))
		{
			return false;
		}
	}

	return true;
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



void UActorInventoryManagerComponent::SetInventoryWidgetClass(const TSubclassOf<UInventoryWidget> NewInventoryWidgetClass)
{
	InventoryWidgetClass = NewInventoryWidgetClass;
}

void UActorInventoryManagerComponent::SetInventoryCategoryWidgetClass(
	const TSubclassOf<UInventoryCategoryWidget> NewInventoryCategoryWidgetClass)
{
	InventoryCategoryClass = NewInventoryCategoryWidgetClass;
}

void UActorInventoryManagerComponent::SetInventoryItemSlotWidgetClass(
	const TSubclassOf<UInventoryItemSlot> NewInventoryItemSlotClass)
{
	InventorySlotClass = NewInventoryItemSlotClass;
}

void UActorInventoryManagerComponent::SetInventoryItemTooltipWidgetClass(
	const TSubclassOf<UInventoryItemSlotTooltip> NewInventoryItemSlotTooltipClass)
{
	InventorySlotTooltipClass = NewInventoryItemSlotTooltipClass;
}

void UActorInventoryManagerComponent::SetInventoryItemDragDropWidgetClass(
	const TSubclassOf<UInventoryItemSlotDrag> NewInventoryItemSlotDragDropClass)
{
	InventorySlotDragClass = NewInventoryItemSlotDragDropClass;
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
