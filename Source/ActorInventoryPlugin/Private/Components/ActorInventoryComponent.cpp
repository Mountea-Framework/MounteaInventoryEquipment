// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryComponent.h"

#include "Definitions/InventoryCategory.h"
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

void UActorInventoryComponent::SetInventoryLayout(const FInventoryLayout& InInventoryLayout)
{
	InventoryLayout = InInventoryLayout;
}

void UActorInventoryComponent::SaveToInventoryLayout(FIntPoint& SlotCoordinates, FInventorySlotData& Slot)
{
	if (InventoryLayout.SavedInventoryLayout.Contains(SlotCoordinates))
	{
		InventoryLayout.SavedInventoryLayout.Remove(SlotCoordinates);
		InventoryLayout.SavedInventoryLayout.Add(SlotCoordinates, Slot);
	}
	else
	{
		InventoryLayout.SavedInventoryLayout.Add(SlotCoordinates, Slot);
	}
}

void UActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryManager = UActorInventoryBPFLibrary::GetInventoryManager(this);
}

void UActorInventoryComponent::SplitItemByCategory(FInventoryItemData& NewItemData, FInventoryItemData& ExistingItemData)
{
	if (NewItemData.ItemCategory && ExistingItemData.ItemCategory)
	{
		// Calculate how much to Allowed Maximum is left
		int32 AmountToLeave = NewItemData.ItemQuantityData.Quantity - (ExistingItemData.ItemCategory->GetMaxQuantityPerStack() - ExistingItemData.ItemQuantityData.Quantity);
		
		// Add up to Maximum Allowed Stack
		ExistingItemData.ItemQuantityData.Quantity = ExistingItemData.ItemCategory->GetMaxQuantityPerStack();

		while (AmountToLeave > 0)
		{
			FInventoryItemData CreatedItemData = NewItemData;
			//CreatedItemData.ForceGUID(NewItemData.GetGuid());

			//TODO: GUIDS!
			// Subtract added from New Item
			CreatedItemData.ItemQuantityData.Quantity = FMath::Min(ExistingItemData.ItemCategory->GetMaxQuantityPerStack(), AmountToLeave);

			UInventoryItem* SplitItem = NewObject<UInventoryItem>();
			SplitItem->SetItem(CreatedItemData);

			AmountToLeave -= ExistingItemData.ItemCategory->GetMaxQuantityPerStack();
						
			AddItemToInventory(SplitItem);
		}
	}
}

bool UActorInventoryComponent::AddItemToInventory(UInventoryItem* Item)
{
	EInventoryContext InventoryContext = EInventoryContext::Default;

	// Invalid Ptr
	if (!Item)
	{
		InventoryContext = EInventoryContext::EIC_Failed_InvalidItem;
		OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
		return false;
	}

	// Invalid Item
	if (!(Item->IsValidItem()))
	{
		InventoryContext = EInventoryContext::EIC_Failed_InvalidItem;
		OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
		return false;
	}

	// Cache data
	FInventoryItemData& AddingItemData = Item->GetItem();
	const int32 ProcessAmount = AddingItemData.ItemQuantityData.Quantity;

	UInventoryItem* ExistingItem = nullptr;
	if(FindItemByGUID(AddingItemData.GetGuid()))
	{
		ExistingItem = GetItemByGUID(AddingItemData.GetGuid());
	}
	else if (FindItemByData(AddingItemData))
	{
		ExistingItem = GetItemByData(AddingItemData);
	}
	else if (FindItemByClass(Item->StaticClass()))
	{
		ExistingItem = GetItemByClass(Item->StaticClass());
	}
	
	if (ExistingItem)
	{
		FInventoryItemData& ExistingItemData = ExistingItem->GetItem();
		
		const int32 PredictedAmount = ExistingItemData.ItemQuantityData.Quantity + ProcessAmount;

		const bool bIsStacking = ExistingItemData.ItemQuantityData.bIsStackable;
		const bool bWouldReachLimit =
		(
			PredictedAmount
			>
			ExistingItemData.ItemQuantityData.MaxQuantity
		);
		const bool bWouldApplyToLimit =
		(
			PredictedAmount == ExistingItemData.ItemQuantityData.MaxQuantity
		);
		
		// Decisions
		if (bIsStacking) // Can Stack, set Quantity to Max and maybe Leave something
		{
			if (bWouldReachLimit)
			{
				AddingItemData.ItemQuantityData.Quantity = PredictedAmount - ExistingItemData.ItemQuantityData.MaxQuantity;
				ExistingItemData.ItemQuantityData.Quantity = ExistingItemData.ItemQuantityData.MaxQuantity;
				
				InventoryContext = EInventoryContext::EIC_Failed_LimitReached;
				OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);

				return false;
			}
			else if (bWouldApplyToLimit)
			{
				InventoryContext = EInventoryContext::EIC_Success;
				OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
				
				ExistingItemData.ItemQuantityData.Quantity += ProcessAmount;

				return true;
			}
			else
			{
				if (ExistingItemData.ItemCategory)
				{
					if(PredictedAmount > ExistingItemData.ItemCategory->GetMaxQuantityPerStack())
					{
						// TODO: Split should not affect InventoryItems array, but there should be layout data only
						SplitItemByCategory(AddingItemData, ExistingItemData);
						
						InventoryContext = EInventoryContext::EIC_Success_SplitStack;
						OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
						
						return true;
					}
					else
					{
						InventoryContext = EInventoryContext::EIC_Success;
						OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
				
						ExistingItemData.ItemQuantityData.Quantity += ProcessAmount;

						return true;
					}
				}
				else 
				{
					InventoryContext = EInventoryContext::EIC_Failed_InvalidItem;
					OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
					return false;
				}
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
			AddingItemData.ItemQuantityData.Quantity
			>
			AddingItemData.ItemQuantityData.MaxQuantity
		);

		if (bLimitReached) // Clamp
		{
			FInventoryItemData NewItemData = AddingItemData;
			
			if (AddingItemData.ItemCategory)
			{
				SplitItemByCategory(AddingItemData, NewItemData);
			}
			
			NewItemData.ItemQuantityData.Quantity = FMath::Min(AddingItemData.ItemQuantityData.Quantity, AddingItemData.ItemQuantityData.MaxQuantity);
			AddingItemData.ItemQuantityData.Quantity -= NewItemData.ItemQuantityData.Quantity;

			InventoryContext = EInventoryContext::EIC_Success_SplitStack;
			OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
			
			UInventoryItem* NewItem = NewObject<UInventoryItem>();
			NewItem->SetItem(NewItemData);
		
			InventoryItems.Emplace(NewItem);

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

void UActorInventoryComponent::SubtractItemFromInventory(UInventoryItem* Item, int32 Amount)
{
	EInventoryContext InventoryContext = EInventoryContext::Default;
	
	if (!Item)
	{
		InventoryContext = EInventoryContext::EIC_Failed_InvalidItem;
		OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
		
		return;
	}

	// Cache data
	const FInventoryItemData& NewItemData = Item->GetItem();
	
	if (UInventoryItem* ExistingItem = GetItemByData(NewItemData))
	{
		if (ExistingItem->GetItem().ItemQuantityData.Quantity - Amount <= 0)
		{
			RemoveItemFromInventory(ExistingItem);
		}
		else
		{
			ExistingItem->GetItem().ItemQuantityData.Quantity -= Amount;
		}

		InventoryContext = EInventoryContext::EIC_Success_RemovedItem;
		OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
	}
}

void UActorInventoryComponent::SubtractItemsFromInventory(const TMap<UInventoryItem*, int32>& ListOfItems)
{
	for (const auto Itr : ListOfItems)
	{
		SubtractItemFromInventory(Itr.Key, Itr.Value);
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
	for (const auto Itr : InventoryItems)
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
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem().CompareGuid(Guid))
		{
			return true;
		}
	}

	return false;
}

bool UActorInventoryComponent::FindItemByData(const FInventoryItemData& Data) const
{
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem() == Data)
		{
			return true;
		}
	}
	return false;
}

UInventoryItem* UActorInventoryComponent::GetItemByData(const FInventoryItemData& ItemData) const
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

TArray<UInventoryItem*> UActorInventoryComponent::GetItemsByData(const FInventoryItemData& ItemData) const
{
	TArray<UInventoryItem*> ReturnItems;
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem() == ItemData && !ReturnItems.Contains(Itr))
		{
			ReturnItems.Add(Itr);
		}
	}

	return ReturnItems;
}

UInventoryItem* UActorInventoryComponent::GetItemByGUID(const FGuid& Guid) const
{
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem().CompareGuid(Guid))
		{
			return Itr;
		}
	}

	return nullptr;
}

TArray<UInventoryItem*> UActorInventoryComponent::GetItemsByGUID(const FGuid& Guid) const
{
	TArray<UInventoryItem*> ReturnItems;
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->GetItem().CompareGuid(Guid) && !ReturnItems.Contains(Itr))
		{
			ReturnItems.Add(Itr);
		}
	}

	return ReturnItems;
}

UInventoryItem* UActorInventoryComponent::GetItemByClass(const TSubclassOf<UInventoryItem>& Class) const
{
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->StaticClass() == Class)
		{
			return Itr;
		}
	}
	return nullptr;
}

TArray<UInventoryItem*> UActorInventoryComponent::GetItemsByClass(const TSubclassOf<UInventoryItem>& Class) const
{
	TArray<UInventoryItem*> ReturnItems;
	for (const auto Itr : InventoryItems)
	{
		if (Itr && Itr->StaticClass() == Class && !ReturnItems.Contains(Itr))
		{
			ReturnItems.Add(Itr);
		}
	}

	return ReturnItems;
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

FOnInventoryLayoutSaveRequested& UActorInventoryComponent::GetInventoryLayoutUpdateRequestHandle()
{
	return OnInventoryLayoutSaveRequested;
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
