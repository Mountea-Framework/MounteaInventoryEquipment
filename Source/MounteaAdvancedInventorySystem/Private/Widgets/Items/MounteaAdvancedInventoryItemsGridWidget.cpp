// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Widgets/Items/MounteaAdvancedInventoryItemsGridWidget.h"

#include "Algo/Accumulate.h"
#include "Algo/AnyOf.h"
#include "Algo/Copy.h"
#include "Algo/Find.h"

#include "Interfaces/Inventory/MounteaAdvancedInventoryInterface.h"
#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotWidgetInterface.h"

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

bool UMounteaAdvancedInventoryItemsGridWidget::RemoveItemFromGrid_Implementation(const FGuid& ItemId, const int32 Quantity)
{
	if (!ItemId.IsValid()) return false;
	
	auto allSlots = Execute_GetGridSlotsDataForItem(this, ItemId);
	if (allSlots.Num() == 0) return false;
	
	// If Quantity is -1 or 0, remove all
	if (Quantity <= 0)
	{
		bool bSuccess = true;
		for (const auto& slot : allSlots)
		{
			int32 slotIndex = Execute_GetSlotIndexByItem(this, slot.OccupiedItemId);
			if (slotIndex != INDEX_NONE)
			{
				if (!Execute_RemoveItemFromSlot(this, slotIndex))
					bSuccess = false;
			}
		}
		return bSuccess;
	}

	allSlots.Sort([](const FMounteaInventoryGridSlot& a, const FMounteaInventoryGridSlot& b) {
		if (a.SlotPosition.Y != b.SlotPosition.Y)
			return a.SlotPosition.Y > b.SlotPosition.Y; // Sort by Y descending first
		return a.SlotPosition.X > b.SlotPosition.X; // Then by X descending
	});
	
	int32 remainingQuantity = Quantity;
	bool bAnyRemoved = false;
	
	for (const auto& slot : allSlots)
	{
		if (remainingQuantity <= 0) break;
		
		int32 slotIndex = Execute_GetSlotIndexByItem(this, slot.OccupiedItemId);
		if (slotIndex == INDEX_NONE) continue;
		
		if (slot.SlotQuantity <= remainingQuantity)
		{
			// Remove entire slot
			if (Execute_RemoveItemFromSlot(this, slotIndex))
			{
				remainingQuantity -= slot.SlotQuantity;
				bAnyRemoved = true;
			}
		}
		else
		{
			// Reduce quantity in slot
			FMounteaInventoryGridSlot updatedSlot = slot;
			updatedSlot.SlotQuantity -= remainingQuantity;
			GridSlots.Emplace(updatedSlot);
			ParentUIComponent->Execute_UpdateSlot(ParentUIComponent.GetObject(), updatedSlot);
			
			if (IsValid(updatedSlot.SlotWidget))
				IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(updatedSlot.SlotWidget);
				
			remainingQuantity = 0;
			bAnyRemoved = true;
		}
	}
	
	return bAnyRemoved;
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

int32 UMounteaAdvancedInventoryItemsGridWidget::GetGridSlotIndexByCoords_Implementation(
	const FIntPoint& SlotCoords) const
{
	const FMounteaInventoryGridSlot* FoundSlot = Algo::FindByPredicate(GridSlots,
		[&SlotCoords](const FMounteaInventoryGridSlot& checkedSlot)
		{
			return checkedSlot.SlotPosition == SlotCoords;
		});

	return FoundSlot ? GridSlots.Array().IndexOfByKey(*FoundSlot) : -1;
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
	if (GridSlots.Array().IsValidIndex(SlotIndex) == false)
		return FMounteaInventoryGridSlot();
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

bool UMounteaAdvancedInventoryItemsGridWidget::UpdateItemInSlot_Implementation(const FGuid& ItemId, const int32 SlotIndex)
{
	return UMounteaInventoryUIStatics::Helper_ItemsGrid_UpdateItemInSlot(this, ItemId, SlotIndex, ParentUIComponent);
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
