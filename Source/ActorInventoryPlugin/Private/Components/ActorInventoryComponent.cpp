// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryBPFLibrary.h"
#include "Helpers/ActorInventoryPluginLog.h"
#include "Widgets/InventoryWidget.h"

UActorInventoryComponent::UActorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	InventoryWidgetClass = UInventoryWidget::StaticClass();
}

void UActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryManager = UActorInventoryBPFLibrary::GetInventoryManager(this);
}

bool UActorInventoryComponent::AddItemToInventory(UInventoryItem* Item)
{
	EInventoryContext InventoryContext = EInventoryContext::Default;
	
	if (!Item)
	{
		InventoryContext = EInventoryContext::EIC_Failed_InvalidItem;
		OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
		return false;
	}

	// Cache data
	FInventoryItemData& NewItemData = Item->GetItem();
	const int32 ProcessAmount = NewItemData.ItemQuantityData.ProcessAmount;
	
	if (UInventoryItem* ExistingItem = GetItemFromInventory(NewItemData))
	{
		FInventoryItemData& ExistingItemData = ExistingItem->GetItem();

		const bool bIsStacking = ExistingItemData.ItemQuantityData.bIsStackable;
		const bool bLimitReached =
		(
			NewItemData.ItemQuantityData.ProcessAmount + ExistingItemData.ItemQuantityData.Quantity
			>
			ExistingItemData.ItemQuantityData.MaxQuantity
		);

		// Decisions
		if (bIsStacking) // Can Stack, validate over MaxQuantity
		{
			if (bLimitReached)
			{
				const int32 QuantityToAdd = ExistingItemData.ItemQuantityData.MaxQuantity - ExistingItemData.ItemQuantityData.Quantity;
				
				if (QuantityToAdd == 0)
				{
					InventoryContext = EInventoryContext::EIC_Failed_LimitReached;
					OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);

					return false;
				}

				InventoryContext = EInventoryContext::EIC_Success_SplitStack;
				OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);

				ExistingItemData.ItemQuantityData.Quantity = FMath::Min
				(
					ExistingItemData.ItemQuantityData.Quantity + NewItemData.ItemQuantityData.ProcessAmount,
					ExistingItemData.ItemQuantityData.MaxQuantity
				);

				// Subtract added from New Item
				NewItemData.ItemQuantityData.Quantity =- QuantityToAdd;
				
				return false;
			}
			else
			{

				InventoryContext = EInventoryContext::EIC_Success;
				OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
				
				ExistingItemData.ItemQuantityData.Quantity += ProcessAmount;

				return true;
			}
		}
		else // Cannot Stack
		{
			InventoryContext = EInventoryContext::EIC_Failed_LimitReached;
			OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);

			return false;
		}
	}
	else
	{
		const bool bLimitReached =
		(
			NewItemData.ItemQuantityData.ProcessAmount
			>
			NewItemData.ItemQuantityData.MaxQuantity
		);

		if (bLimitReached) // Clamp
		{
			NewItemData.ItemQuantityData.Quantity = FMath::Min(NewItemData.ItemQuantityData.ProcessAmount, NewItemData.ItemQuantityData.MaxQuantity);

			InventoryContext = EInventoryContext::EIC_Success_SplitStack;
			OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
			InventoryItems.Emplace(Item);

			return false;
		}
		else // Cannot Stack
		{
			InventoryContext = EInventoryContext::EIC_Success;
			OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
			InventoryItems.Emplace(Item);

			return true;
		}
	}

	return false;
}

bool UActorInventoryComponent::AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems)
{
	
	for (UInventoryItem* Itr : ListOfItems)
	{
		if (!AddItemToInventory(Itr))
		{
			return false;
		}
	}

	OnInventoryUpdated.Broadcast(this);
	return true;
}

void UActorInventoryComponent::RemoveItemsFromInventory(const TArray<UInventoryItem*>& ListOfItems)
{
	for (auto& Itr : ListOfItems)
	{
		RemoveItemFromInventory(Itr);
	}
}

void UActorInventoryComponent::RemoveItemFromInventory(UInventoryItem* Item)
{
	if (Item != nullptr && IsItemInInventory(Item))
	{
		InventoryItems.RemoveSingle(Item);
	}
}

void UActorInventoryComponent::SubtractItemFromInventory(UInventoryItem* Item)
{
	FString ContextString = FString("");
	EInventoryContext InventoryContext = EInventoryContext::Default;
	
	if (!Item)
	{
		InventoryContext = EInventoryContext::EIC_Failed_InvalidItem;
		OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
		return;
	}

	// Only Process removing Items and reuse existing function
	if (Item->GetItem().ItemQuantityData.ProcessAmount < 0)
	{
		AddItemToInventory(Item);
	}
}

void UActorInventoryComponent::SubtractItemsFromInventory(const TArray<UInventoryItem*>& ListOfItems)
{
	for (const auto Itr : ListOfItems)
	{
		SubtractItemFromInventory(Itr);
	}

	OnInventoryUpdated.Broadcast(this);
}

int32 UActorInventoryComponent::GetItemQuantity(UInventoryItem* Item) const
{
	if (IsItemInInventory(Item))
	{
		return Item->GetItem().ItemQuantityData.Quantity;
	}
	return 0;
}

bool UActorInventoryComponent::FindItemByClass(const TSubclassOf<UInventoryItem> ItemClass) const
{
	for (const UInventoryItem* Itr : InventoryItems)
	{
		if (Itr && Itr->GetClass() == ItemClass)
		{
			return true;
		}
	}

	return false;
}

bool UActorInventoryComponent::FindItemByGUID(const FGuid& Guid) const
{
	for (UInventoryItem* Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem().GetGuid() == Guid)
		{
			return true;
		}
	}

	return false;
}

UInventoryItem* UActorInventoryComponent::GetItemFromInventory(const FInventoryItemData& ItemData) const
{
	for (auto& Itr: InventoryItems)
	{
		if (Itr && Itr->GetItem() == ItemData)
		{
			return Itr;
		}
	}
	
	return nullptr;
}

bool UActorInventoryComponent::IsItemInInventory(UInventoryItem* Item) const
{
	if (!Item) return false;

	for (UInventoryItem* Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem() == Item->GetItem())
		{
			return true;
		}
	}

	return false;
}

void UActorInventoryComponent::LoadInventoryContent(const UDataTable* SourceTable)
{
	if (SourceTable && SourceTable->RowStruct)
	{
		const UScriptStruct* InventoryRowStruct = SourceTable->GetRowStruct();
		if (InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct()))
		{
			static const FString ContextString(TEXT("ContextString"));
			for (const FName Itr : SourceTable->GetRowNames())
			{
				const FInventoryItemData* Row = SourceTable->FindRow<FInventoryItemData>(Itr, ContextString);
				
				if (Row)
				{
					UInventoryItem* NewItem = NewObject<UInventoryItem>(UInventoryItem::StaticClass());

					if (NewItem)
					{
						NewItem->SetItem(*Row);
						AddItemToInventory(NewItem);
					}
				}
			}
		} 
	}
}

FOnInventoryUpdated& UActorInventoryComponent::GetUpdateEventHandle()
{
	return OnInventoryUpdated;
}

FOnInventoryUpdateRequestProcessed& UActorInventoryComponent::GetInventoryRequestProcessedHandle()
{
	return OnInventoryUpdateRequestProcessed;
}

void UActorInventoryComponent::SetInventoryWidgetClass(const TSubclassOf<UInventoryWidget> NewInventoryWidgetClass)
{
	InventoryWidgetClass = NewInventoryWidgetClass;
}

void UActorInventoryComponent::SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget)
{
	InventoryWidget = NewInventoryWidget;
}

UInventoryWidget* UActorInventoryComponent::GetInventoryWidgetPtr() const
{
	return InventoryWidget;
}

TSubclassOf<UInventoryWidget> UActorInventoryComponent::GetInventoryWidgetClass() const
{
	return InventoryWidgetClass;
}

void UActorInventoryComponent::SetNotificationContainerPtr(UInventoryNotificationContainer* Widget)
{
	InventoryNotificationContainer = Widget;
}
