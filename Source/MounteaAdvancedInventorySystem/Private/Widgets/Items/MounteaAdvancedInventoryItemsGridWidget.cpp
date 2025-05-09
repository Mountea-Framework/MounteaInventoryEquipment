// All rights reserved Dominik Morse 2024


#include "Widgets/Items/MounteaAdvancedInventoryItemsGridWidget.h"

#include "Algo/Accumulate.h"
#include "Algo/AnyOf.h"
#include "Algo/Copy.h"
#include "Algo/Find.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"

#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotWidgetInterface.h"
#include "Statics/MounteaInventoryStatics.h"
#include "Statics/MounteaInventoryUIStatics.h"

bool UMounteaAdvancedInventoryItemsGridWidget::AddItemToEmptySlot_Implementation(const FGuid& ItemId)
{
	const int32 EmptySlotIndex = Execute_FindEmptySlotIndex(this, ItemId);
	if (!GridSlots.Array().IsValidIndex(EmptySlotIndex)) return false;

	return Execute_AddItemToSlot(this, ItemId, EmptySlotIndex);
}

bool UMounteaAdvancedInventoryItemsGridWidget::AddItemToSlot_Implementation(const FGuid& ItemId, const int32 SlotIndex)
{
	if (!Execute_IsSlotEmpty(this, SlotIndex)) return false;
	const auto parentInventory = ParentUIComponent->Execute_GetParentInventory(ParentUIComponent.GetObject());
	if (parentInventory.GetObject() == nullptr) return false;
	const auto item = parentInventory->Execute_FindItem(parentInventory.GetObject(), FInventoryItemSearchParams(ItemId));
	if (!item.IsItemValid()) return false;
	
	FMounteaInventoryGridSlot tempGridSlot = GridSlots.Array()[SlotIndex];

	const TObjectPtr<UUserWidget>& slotWidget = tempGridSlot.SlotWidget;
	if (!IsValid(slotWidget)) return false;
	if (!slotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) return false;
		
	tempGridSlot.OccupiedItemId = ItemId;
	tempGridSlot.SlotQuantity = item.GetQuantity();
	GridSlots.Emplace(tempGridSlot);

	IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_AddItemToSlot(slotWidget, ItemId);
	IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(slotWidget, tempGridSlot);

	ParentUIComponent->Execute_AddSlot(ParentUIComponent.GetObject(), tempGridSlot);

	IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(slotWidget);

	return true;
}

bool UMounteaAdvancedInventoryItemsGridWidget::RemoveItemFromSlot_Implementation(const int32 SlotIndex)
{
	if (!GridSlots.Array().IsValidIndex(SlotIndex)) return false;

	FMounteaInventoryGridSlot tempGridSlot = GridSlots.Array()[SlotIndex];

	const TObjectPtr<UUserWidget>& slotWidget = tempGridSlot.SlotWidget;
	if (!IsValid(slotWidget)) return false;
	if (!slotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) return false;

	const FGuid itemId = tempGridSlot.OccupiedItemId;
	IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_RemoveItemFromSlot(slotWidget, itemId);

	tempGridSlot.ResetSlot();
	IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(slotWidget, tempGridSlot);
	ParentUIComponent->Execute_UpdateSlot(ParentUIComponent.GetObject(), tempGridSlot);
	
	GridSlots.Emplace(tempGridSlot);

	IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(slotWidget);

	return true;
}

FGuid UMounteaAdvancedInventoryItemsGridWidget::GetItemInSlot_Implementation(const int32 SlotIndex) const
{
	if (!GridSlots.Array().IsValidIndex(SlotIndex)) return FGuid();
	const FMounteaInventoryGridSlot gridSlot = GridSlots.Array()[SlotIndex];
	return gridSlot.OccupiedItemId;
}

bool UMounteaAdvancedInventoryItemsGridWidget::SwapItemsBetweenSlots_Implementation(const int32 SlotIndex1, const int32 SlotIndex2)
{
	if (!GridSlots.Array().IsValidIndex(SlotIndex1) || !GridSlots.Array().IsValidIndex(SlotIndex2))
		return false;

	FMounteaInventoryGridSlot tempSlot1 = GridSlots.Array()[SlotIndex1];
	FMounteaInventoryGridSlot tempSlot2 = GridSlots.Array()[SlotIndex2];

	GridSlots.Remove(tempSlot1);
	GridSlots.Remove(tempSlot2);

	Swap(tempSlot1.OccupiedItemId, tempSlot2.OccupiedItemId);

	GridSlots.Add(tempSlot1);
	GridSlots.Add(tempSlot2);

	if (IsValid(tempSlot1.SlotWidget) && tempSlot1.SlotWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(tempSlot1.SlotWidget);

	if (IsValid(tempSlot2.SlotWidget) && tempSlot2.SlotWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(tempSlot2.SlotWidget);

	return true;
}

void UMounteaAdvancedInventoryItemsGridWidget::ClearAllSlots_Implementation()
{
	TSet<FMounteaInventoryGridSlot> clearGridSlots;
	for (const auto& gridSlot : GridSlots)
	{
		FMounteaInventoryGridSlot tempGridSlot = gridSlot;
		tempGridSlot.OccupiedItemId = FGuid();
		const TObjectPtr<UUserWidget>& slotWidget = tempGridSlot.SlotWidget;
		if (IsValid(slotWidget) && slotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>())
			IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(slotWidget, tempGridSlot);
		clearGridSlots.Add(tempGridSlot);
	}

	GridSlots.Empty();
	GridSlots.Append(clearGridSlots);
}

int32 UMounteaAdvancedInventoryItemsGridWidget::GetTotalSlots_Implementation() const
{
	return GridSlots.Num();
}

bool UMounteaAdvancedInventoryItemsGridWidget::IsSlotEmpty_Implementation(const int32 SlotIndex) const
{
	if (!GridSlots.Array().IsValidIndex(SlotIndex)) return true;

	return GridSlots.Array()[SlotIndex].IsEmpty();
}

int32 UMounteaAdvancedInventoryItemsGridWidget::GetSlotIndexByItem_Implementation(const FGuid& ItemId) const
{
	const TArray<FMounteaInventoryGridSlot>& Slots = GridSlots.Array();

	const FMounteaInventoryGridSlot* FoundSlot = Algo::FindByPredicate(Slots,
		[&ItemId](const FMounteaInventoryGridSlot& checkedSlot)
		{
			return checkedSlot.OccupiedItemId == ItemId;
		});

	return FoundSlot ? Slots.IndexOfByKey(*FoundSlot) : -1;
}

bool UMounteaAdvancedInventoryItemsGridWidget::IsItemInGrid_Implementation(const FGuid& ItemId) const
{
	return Algo::AnyOf(GridSlots,
		[&ItemId](const FMounteaInventoryGridSlot& checkedSlot)
		{
			return checkedSlot.OccupiedItemId == ItemId;
		});
}

FMounteaInventoryGridSlot UMounteaAdvancedInventoryItemsGridWidget::GetGridSlotData_Implementation(
	const int32 SlotIndex) const
{
	return GridSlots.Array()[SlotIndex];
}

TSet<FMounteaInventoryGridSlot> UMounteaAdvancedInventoryItemsGridWidget::GetGridSlotsData_Implementation() const
{
	return GridSlots;
}

UUserWidget* UMounteaAdvancedInventoryItemsGridWidget::FindEmptyWidgetSlot_Implementation() const
{
	const FMounteaInventoryGridSlot* EmptySlot = Algo::FindByPredicate(GridSlots,
		[](const FMounteaInventoryGridSlot& checkedSlot)
		{
			return checkedSlot.IsEmpty();
		});

	return EmptySlot ? EmptySlot->SlotWidget : nullptr;
}

int32 UMounteaAdvancedInventoryItemsGridWidget::FindEmptySlotIndex_Implementation(const FGuid& ItemId) const
{
	return UMounteaInventoryUIStatics::Helper_FindEmptyGridSlotIndex(this, ItemId, ParentUIComponent.GetObject() );
}

UUserWidget* UMounteaAdvancedInventoryItemsGridWidget::GetItemSlotWidget_Implementation(const int32 SlotIndex) const
{
	if (!GridSlots.Array().IsValidIndex(SlotIndex)) return nullptr;

	return GridSlots.Array()[SlotIndex].SlotWidget;
}

UUserWidget* UMounteaAdvancedInventoryItemsGridWidget::GetItemWidgetInSlot_Implementation(const int32 SlotIndex) const
{
	if (!GridSlots.Array().IsValidIndex(SlotIndex)) return nullptr;

	const FMounteaInventoryGridSlot& checkedSlot = GridSlots.Array()[SlotIndex];

	if (!IsValid(checkedSlot.SlotWidget)) return nullptr;

	if (!checkedSlot.SlotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) return nullptr;

	return IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_GetItemWidgetInSlot(checkedSlot.SlotWidget);
}

void UMounteaAdvancedInventoryItemsGridWidget::AddSlot_Implementation(const FMounteaInventoryGridSlot& SlotData)
{
	GridSlots.Add(SlotData);
}

// TODO: if item is NOT stackable and quantity is greater than 1, return false
// TODO: if no slot has this item, return AddItem to add item
// TODO: if at least one slot has this item, get all slots with item
// SUM quantity in all slots and find difference between current SUM and Quantity in item
// if SUM < item Quantity, we are adding
// we will add as much as we can to provided slot
// if SUM is still less then Quantity, we need to add to all slots with this item - if no slots with this item, we need to start adding
// if all slots with this item are full and we still have something left to add, we start adding
// if SUM > item Quantity, we are removing and we need to reduce quantity in slot until SUM == Quantity
// if we removed quantity in provided slot and SUM is still greater we need to start removing from next slot until SUM == Quantity (recursive search)
// if SUM == item Quantity, we are updating quantity in slot
// we should utilize same logic for Add Item To Slot and Remove Item From Slot
bool UMounteaAdvancedInventoryItemsGridWidget::UpdateItemInSlot_Implementation(const FGuid& ItemId, const int32 SlotIndex)
{
	if (!ItemId.IsValid()) return false;

	if (!IsValid(ParentUIComponent.GetObject())) return false;
	const auto parentInventory = ParentUIComponent->Execute_GetParentInventory(ParentUIComponent.GetObject());
	if (!IsValid(parentInventory.GetObject())) return false;

	const auto item = parentInventory->Execute_FindItem(parentInventory.GetObject(), FInventoryItemSearchParams(ItemId));
	if (!item.IsItemValid()) return false;
	
	const bool bIsStackable = UMounteaInventoryStatics::HasInventoryFlags(
		item.Template->ItemFlags, static_cast<int32>(EInventoryItemFlags::EIIF_Stackable
	));
	
	auto itemSlots = Execute_GetGridSlotsDataForItem(this, ItemId);
	const int32 currentTotalQuantity = Execute_GetStacksSizeForItem(this, ItemId);
	const int32 targetTotalQuantity = item.GetQuantity();
	const int32 maxStackSize = item.Template->MaxStackSize;
	
	if (currentTotalQuantity == targetTotalQuantity && itemSlots.Num() > 0)
	{
		LOG_WARNING(TEXT("[UpdateItem] Item %s is not UI dirty, quantity has not changed"), *item.GetItemName().ToString());
		return false;
	}

	bool bAnySlotUpdated = false;

	// Helper function to update a slot's quantity and refresh UI
	auto updateSlotQuantity = [&](FMounteaInventoryGridSlot& slot, const int32 newQuantity) -> void {
		slot.SlotQuantity = newQuantity;
		GridSlots.Emplace(slot);
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(slot.SlotWidget, slot);
		ParentUIComponent->Execute_UpdateSlot(ParentUIComponent.GetObject(), slot);
		
		if (IsValid(slot.SlotWidget))
			IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(slot.SlotWidget);
			
		bAnySlotUpdated = true;
	};
	
	// Case 1: We need to add items (SUM < item Quantity)
	if (targetTotalQuantity > currentTotalQuantity)
	{
		int32 quantityToAdd = targetTotalQuantity - currentTotalQuantity;
		
		// Handle non-stackable items differently, for non-stackable items, we need one slot per item
		if (!bIsStackable)
		{
			bool bSpecifiedSlotUsed = false;
			
			while (quantityToAdd > 0)
			{
				int32 emptySlotIndex = INDEX_NONE;
				
				// Use specified slot first if available and not used yet
				if (SlotIndex != INDEX_NONE && !bSpecifiedSlotUsed && Execute_IsSlotEmpty(this, SlotIndex))
				{
					emptySlotIndex = SlotIndex;
					bSpecifiedSlotUsed = true;
				}
				else
					emptySlotIndex = Execute_FindEmptySlotIndex(this, ItemId);
				
				if (emptySlotIndex == INDEX_NONE) break; // No more empty slots
				
				if (Execute_AddItemToSlot(this, ItemId, emptySlotIndex))
				{
					FMounteaInventoryGridSlot slotData = Execute_GetGridSlotData(this, emptySlotIndex);
					updateSlotQuantity(slotData, 1); // Each non-stackable item gets quantity of 1
					quantityToAdd--;
					bAnySlotUpdated = true;
				}
				else
				{
					break; // Failed to add to slot
				}
			}
		}
		// Handle stackable items
		else
		{
			// If no slots exist for this item, add it to an empty slot
			if (itemSlots.Num() == 0)
			{
				int32 emptySlotIndex = SlotIndex != INDEX_NONE ? SlotIndex : Execute_FindEmptySlotIndex(this, ItemId);
				if (emptySlotIndex != INDEX_NONE && Execute_IsSlotEmpty(this, emptySlotIndex))
				{
					int32 quantityToAddToSlot = FMath::Min(maxStackSize, quantityToAdd);
					if (Execute_AddItemToSlot(this, ItemId, emptySlotIndex))
					{
						FMounteaInventoryGridSlot slotData = Execute_GetGridSlotData(this, emptySlotIndex);
						updateSlotQuantity(slotData, quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}
			}
			// Otherwise, first try to add to the specified slot if provided
			else if (SlotIndex != INDEX_NONE && GridSlots.Array().IsValidIndex(SlotIndex))
			{
				FMounteaInventoryGridSlot slotData = Execute_GetGridSlotData(this, SlotIndex);
				
				if (slotData.IsEmpty())
				{
					int32 quantityToAddToSlot = FMath::Min(maxStackSize, quantityToAdd);
					if (Execute_AddItemToSlot(this, ItemId, SlotIndex))
					{
						slotData = Execute_GetGridSlotData(this, SlotIndex); // Re-get after adding
						updateSlotQuantity(slotData, quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}
				else if (slotData.OccupiedItemId == ItemId)
				{
					int32 availableSpace = maxStackSize - slotData.SlotQuantity;
					int32 quantityToAddToSlot = FMath::Min(availableSpace, quantityToAdd);
					
					if (quantityToAddToSlot > 0)
					{
						updateSlotQuantity(slotData, slotData.SlotQuantity + quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}

				itemSlots.Remove(slotData);
			}
			
			// Fill up existing item slots
			if (quantityToAdd > 0)
			{
				for (auto& slot : itemSlots)
				{
					if (quantityToAdd <= 0) break;
					
					const int32 availableSpace = maxStackSize - slot.SlotQuantity;
					const int32 quantityToAddToSlot = FMath::Min(availableSpace, quantityToAdd);
					
					if (quantityToAddToSlot > 0)
					{
						updateSlotQuantity(slot, slot.SlotQuantity + quantityToAddToSlot);
						quantityToAdd -= quantityToAddToSlot;
					}
				}
			}
			
			// Create new slots if needed
			while (quantityToAdd > 0)
			{
				int32 emptySlotIndex = Execute_FindEmptySlotIndex(this, ItemId);
				if (emptySlotIndex == INDEX_NONE) break; // No more empty slots
				
				int32 quantityToAddToSlot = FMath::Min(maxStackSize, quantityToAdd);

				if (Execute_AddItemToSlot(this, ItemId, emptySlotIndex))
				{
					FMounteaInventoryGridSlot slotData = Execute_GetGridSlotData(this, emptySlotIndex);
					updateSlotQuantity(slotData, quantityToAddToSlot);
					quantityToAdd -= quantityToAddToSlot;
				}
				else
				{
					break; // Failed to add to slot
				}
			}
		}
	}
	
	// Case 2: We need to remove items (SUM > item Quantity)
	else if (targetTotalQuantity < currentTotalQuantity)
	{
		int32 quantityToRemove = currentTotalQuantity - targetTotalQuantity;
	
		if (SlotIndex != INDEX_NONE && GridSlots.Array().IsValidIndex(SlotIndex))
		{
			FMounteaInventoryGridSlot slotData = Execute_GetGridSlotData(this, SlotIndex);
			if (slotData.OccupiedItemId == ItemId)
			{
				const int32 quantityToRemoveFromSlot = FMath::Min(slotData.SlotQuantity, quantityToRemove);
			
				if (quantityToRemoveFromSlot >= slotData.SlotQuantity)
				{
					Execute_RemoveItemFromSlot(this, SlotIndex);
					bAnySlotUpdated = true;
				}
				else
					updateSlotQuantity(slotData, slotData.SlotQuantity - quantityToRemoveFromSlot);
			
				quantityToRemove -= quantityToRemoveFromSlot;
				itemSlots.Remove(slotData);
			}
		}

		if (quantityToRemove > 0)
		{
			for (auto& slot : itemSlots)
			{
				if (quantityToRemove <= 0) break;
			
				const int32 slotIndex = Execute_GetSlotIndexByItem(this, slot.OccupiedItemId);
				if (slotIndex == INDEX_NONE) continue;
			
				const int32 quantityToRemoveFromSlot = FMath::Min(slot.SlotQuantity, quantityToRemove);
			
				if (quantityToRemoveFromSlot >= slot.SlotQuantity)
				{
					Execute_RemoveItemFromSlot(this, slotIndex);
					bAnySlotUpdated = true;
				}
				else
					updateSlotQuantity(slot, slot.SlotQuantity - quantityToRemoveFromSlot);
			
				quantityToRemove -= quantityToRemoveFromSlot;
			}
		}
	}

	// Case 3: SUM == item Quantity (handled by equality check at beginning)
	else
	{
		return true;
	}

	return bAnySlotUpdated;
}

int32 UMounteaAdvancedInventoryItemsGridWidget::GetStacksSizeForItem_Implementation(const FGuid& ItemId)
{
	if (!ItemId.IsValid()) return INDEX_NONE;
	auto allSlots = Execute_GetGridSlotsDataForItem(this, ItemId);
	
	return Algo::Accumulate(allSlots, 0, [&ItemId](const int32 Sum, const FMounteaInventoryGridSlot& gridSlot) {
		return Sum + (gridSlot.OccupiedItemId == ItemId ? gridSlot.SlotQuantity : 0);
	});
}

TSet<FMounteaInventoryGridSlot> UMounteaAdvancedInventoryItemsGridWidget::GetGridSlotsDataForItem_Implementation(
	const FGuid& ItemId)
{
	TSet<FMounteaInventoryGridSlot> returnValue;
	returnValue.Reserve(GridSlots.Num());
	
	Algo::CopyIf(GridSlots, returnValue, [&ItemId](const FMounteaInventoryGridSlot& gridSlot) {
		return gridSlot.OccupiedItemId == ItemId;
	});
	
	return returnValue;
}
