// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryComponent.h"

#include "Definitions/InventoryCategory.h"
#include "Definitions/InventoryItem.h"
#include "Definitions/InventoryKeyAction.h"
#include "Helpers/ActorInventoryBPFLibrary.h"
#include "Helpers/ActorInventoryPluginLog.h"
#include "Widgets/InventoryItemSlot.h"

UActorInventoryComponent::UActorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryManager = UActorInventoryBPFLibrary::GetInventoryManager(this);
	if (!InventoryManager)
	{
		AInvP_LOG(Error, TEXT("[UActorInventoryComponent] Cannot find Inventory Manager! Attach Inventory Manager Component to GameState class!"))

		Deactivate();
	}

	// Bind Virtual function and call BP Event
	OnItemActionRequested.AddUniqueDynamic(this, &UActorInventoryComponent::ExecuteItemKeyAction);
}

void UActorInventoryComponent::SetInventoryLayout(const FInventoryLayout& InInventoryLayout)
{
	InventoryLayout = InInventoryLayout;
}

void UActorInventoryComponent::SaveToInventoryLayoutBP_Implementation(const FInventorySlotData& Slot)
{
	SaveToInventoryLayout(Slot);
}

void UActorInventoryComponent::SaveToInventoryLayout(const FInventorySlotData& Slot)
{
	// Check whether SavedLayout Contains any Values
	if (InventoryLayout.SavedInventoryLayout.Contains(Slot.SlotCoordinates))
	{
		// Validate Slot
		// If Valid, override Slot data
		if (Slot.Item != nullptr && Slot.Quantity > 0 && Slot.bCreatedManually)
		{
			InventoryLayout.SavedInventoryLayout[Slot.SlotCoordinates] = Slot;
		}
		// If Invalid, remove
		else
		{
			InventoryLayout.SavedInventoryLayout.Remove(Slot.SlotCoordinates);
		}
	}
	else
	{
		if (Slot.bCreatedManually)
		{
			InventoryLayout.SavedInventoryLayout.Add(Slot.SlotCoordinates, Slot);
		}
	}
}

void UActorInventoryComponent::ExecuteItemKeyActionBP_Implementation(UInventoryItemSlot* ForItem, const FGuid& ActionGuid)
{
	if (ForItem && ForItem->GetItemData().Item)
	{
		const auto ActionToExecute = UActorInventoryBPFLibrary::FindKeyAction( this, ActionGuid, ForItem->GetItemData().Item->GetItem().ItemCategory);
		if (ActionToExecute)
		{
			ActionToExecute.GetDefaultObject()->ExecuteAction(ForItem);
		}
	}
}

void UActorInventoryComponent::ExecuteItemKeyAction(UInventoryItemSlot* ForItem, const FGuid& ActionGuid)
{
	ExecuteItemKeyActionBP(ForItem, ActionGuid);
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

	// Get Existing Item
	UInventoryItem* ExistingItem = nullptr;
	if(FindItemByGUID(AddingItemData.GetGuid()))
	{
		ExistingItem = GetItemByGUID(AddingItemData.GetGuid());
	}
	else if (FindItemByData(AddingItemData))
	{
		ExistingItem = GetItemByData(AddingItemData);
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
			if (bWouldReachLimit) // Add to Max, leave something
			{
				AddingItemData.ItemQuantityData.Quantity = PredictedAmount - ExistingItemData.ItemQuantityData.MaxQuantity;
				ExistingItemData.ItemQuantityData.Quantity = ExistingItemData.ItemQuantityData.MaxQuantity;
				
				InventoryContext = EInventoryContext::EIC_Failed_LimitReached;
				OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);

				return false;
			}
			else if (bWouldApplyToLimit) // Add to Max, leave nothing
			{
				InventoryContext = EInventoryContext::EIC_Success;
				OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
				
				ExistingItemData.ItemQuantityData.Quantity = ExistingItemData.ItemQuantityData.MaxQuantity;
				AddingItemData.ItemQuantityData.Quantity = 0;

				return true;
			}
			else // Add all
			{
				if (ExistingItemData.ItemCategory)
				{
					if(PredictedAmount > ExistingItemData.ItemCategory->GetMaxQuantityPerStack())
					{
						InventoryContext = EInventoryContext::EIC_Success_SplitStack;
					}
					else
					{
						InventoryContext = EInventoryContext::EIC_Success;
					}
					
					OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
					ExistingItemData.ItemQuantityData.Quantity += ProcessAmount;
				
					return true;

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
			
			NewItemData.ItemQuantityData.Quantity = FMath::Min(AddingItemData.ItemQuantityData.Quantity, AddingItemData.ItemQuantityData.MaxQuantity);
			AddingItemData.ItemQuantityData.Quantity -= NewItemData.ItemQuantityData.Quantity;

			InventoryContext = EInventoryContext::EIC_Success_SplitStack;
			OnInventoryUpdateRequestProcessed.Broadcast(InventoryContext);
			
			UInventoryItem* NewItem = NewObject<UInventoryItem>();
			NewItem->SetItem(NewItemData);
			NewItem->SetOwningInventory(this);
		
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

UActorInventoryComponent* UActorInventoryComponent::GetOtherInventory() const
{
	return OtherInventory;
}

void UActorInventoryComponent::SetOtherInventory(UActorInventoryComponent* NewOtherInventory)
{
	OtherInventory = NewOtherInventory;
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
						NewItem->SetOwningInventory(this);
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

FOnItemActionRequested& UActorInventoryComponent::GetItemActionRequestedHandle()
{
	return OnItemActionRequested;
}

void UActorInventoryComponent::SetInventoryWidgetPtr(UInventoryWidget* NewInventoryWidget)
{
	InventoryWidget = NewInventoryWidget;
}

UInventoryWidget* UActorInventoryComponent::GetInventoryWidgetPtr() const
{
	return InventoryWidget;
}

void UActorInventoryComponent::SetNotificationContainerPtr(UInventoryNotificationContainer* Widget)
{
	InventoryNotificationContainer = Widget;
}


