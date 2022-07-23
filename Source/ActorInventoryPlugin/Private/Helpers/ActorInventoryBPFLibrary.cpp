// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Helpers/ActorInventoryBPFLibrary.h"

#include "Components/ActorInventoryManagerComponent.h"
#include "Definitions/InventoryItem.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

UActorInventoryManagerComponent* UActorInventoryBPFLibrary::GetInventoryManager(const UObject* WorldContextObject)
{
	if (const AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContextObject))
	{
		if (UActorInventoryManagerComponent* InventoryManager = Cast<UActorInventoryManagerComponent>(GameState->GetComponentByClass(UActorInventoryManagerComponent::StaticClass())))
		{
			return InventoryManager;
		}
	}
	
	return nullptr;
}

bool UActorInventoryBPFLibrary::HasValidItem(const FInventorySlotData& ItemData)
{
	return ItemData.Item != nullptr && ItemData.Item->IsValidItem();
}

bool UActorInventoryBPFLibrary::IsValidCategory(const UObject* WorldContextObject, UInventoryCategory* Category)
{
	if (Category)
	{
		if (const UActorInventoryManagerComponent* InventoryManager = GetInventoryManager(WorldContextObject))
		{
			if (InventoryManager->ContainsAllowedCategory(Category))
			{
				if (!Category->GetCategoryData().CategoryName.IsEmpty())
				{
					if (Category->GetCategoryData().IsAllCategories())
					{
						return GetGenericCategory(Category) == Category;
					}
					
					return true;
				}
			}
		}
	}
	
	return false;
}

UInventoryCategory* UActorInventoryBPFLibrary::GetGenericCategory(const UObject* WorldContextObject)
{
	const UActorInventoryManagerComponent* InventoryManager = GetInventoryManager(WorldContextObject);
	if (InventoryManager)
	{
		for (const auto Itr : InventoryManager->GetAllowedCategories())
		{
			if (Itr && Itr->GetCategoryData().IsAllCategories())
			{
				return Itr;
			}
		}
	}
		
	return nullptr;
}

