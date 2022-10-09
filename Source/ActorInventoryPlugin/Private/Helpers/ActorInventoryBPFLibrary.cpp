// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Helpers/ActorInventoryBPFLibrary.h"

#include "Components/ActorInventoryManagerComponent.h"
#include "Definitions/InventoryItem.h"
#include "Definitions/InventoryKeyAction.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/InventoryItemSlot.h"
#include "Widgets/InventoryWidget.h"

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

TSubclassOf<UInventoryKeyAction> UActorInventoryBPFLibrary::FindKeyAction(
	const UObject* WorldContextObject, const FGuid& Guid, const UInventoryCategory* Category)
{
	if (Category)
	{
		if (IsValidKeyAction(WorldContextObject, Guid))
		{
			for (auto Itr : Category->GetCategoryKeyActions())
			{
				if (Itr.Get() && Itr.GetDefaultObject()->GetActionGuid() == Guid)
				{
					return Itr;
				}
				/* No longer using Objects
				if (Itr && Itr->GetActionGuid() == Guid)
				{
					return Itr;
				}
				*/
			}
		}
	}
	
	return nullptr;
}

bool UActorInventoryBPFLibrary::IsValidKeyAction(const UObject* WorldContextObject, const FGuid& Guid)
{
	if (const UActorInventoryManagerComponent* InventoryManager = GetInventoryManager(WorldContextObject))
	{
		TArray<UInventoryCategory*> Categories = InventoryManager->GetAllowedCategories();
		for (const auto ItrCategory : Categories)
		{
			if (ItrCategory)
			{
				auto KeyActions = ItrCategory->GetCategoryKeyActions();

				for (auto Itr : KeyActions)
				{
					if (Itr.Get() && Itr.GetDefaultObject()->GetActionGuid() == Guid)
					{
						return true;
					}
				}
				
				/* No longer using Objects
				for (const UInventoryKeyAction* ItrAction : KeyActions)
				{
					if (ItrAction && ItrAction->GetActionGuid() == Guid)
					{
						return true;
					}
				}
				*/
			}
		}
	}
	return false;
}

