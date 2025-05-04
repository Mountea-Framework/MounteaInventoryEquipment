// All rights reserved Dominik Morse 2024


#include "Widgets/Items/MounteaAdvancedInventoryItemsGridWidget.h"

#include "Algo/AnyOf.h"
#include "Algo/Find.h"

#include "Interfaces/Widgets/Items/MounteaAdvancedInventoryItemSlotWidgetInterface.h"
#include "Statics/MounteaInventoryUIStatics.h"

bool UMounteaAdvancedInventoryItemsGridWidget::AddItemToEmptySlot_Implementation(const FGuid& ItemId)
{
	const int32 EmptySlotIndex = FindEmptySlotIndex();
	if (!GridSlots.Array().IsValidIndex(EmptySlotIndex)) return false;

	return Execute_AddItemToSlot(this, ItemId, EmptySlotIndex);
}

bool UMounteaAdvancedInventoryItemsGridWidget::AddItemToSlot_Implementation(const FGuid& ItemId, const int32 SlotIndex)
{
	if (!Execute_IsSlotEmpty(this, SlotIndex)) return false;

	FMounteaInventoryGridSlot& gridSlot = GridSlots.Array()[SlotIndex];
	const auto slotWidget = gridSlot.SlotWidget;
	if (!IsValid(slotWidget)) return false;
	if (!slotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) return false;
	
	IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_AddItemToSlot(gridSlot.SlotWidget, ItemId);

	gridSlot.OccupiedItemId = ItemId;
	return true;
}

bool UMounteaAdvancedInventoryItemsGridWidget::RemoveItemFromSlot_Implementation(const int32 SlotIndex)
{
	FMounteaInventoryGridSlot& gridSlot = GridSlots.Array()[SlotIndex];
	const auto slotWidget = gridSlot.SlotWidget;
	if (!IsValid(slotWidget)) return false;
	if (!slotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) return false;
	const auto itemId = Execute_GetItemInSlot(this, SlotIndex);
	
	IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_RemoveItemFromSlot(slotWidget, itemId);

	gridSlot.ResetSlot();
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
	if (!GridSlots.Array().IsValidIndex(SlotIndex1) || !GridSlots.Array().IsValidIndex(SlotIndex2)) return false;

	FMounteaInventoryGridSlot& sourceSlot = GridSlots.Array()[SlotIndex1];
	FMounteaInventoryGridSlot& targetSlot = GridSlots.Array()[SlotIndex2];

	const FGuid tempItemId = sourceSlot.OccupiedItemId;
	sourceSlot.OccupiedItemId = targetSlot.OccupiedItemId;
	targetSlot.OccupiedItemId = tempItemId;

	if (IsValid(sourceSlot.SlotWidget) && sourceSlot.SlotWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(sourceSlot.SlotWidget);

	if (IsValid(targetSlot.SlotWidget) && targetSlot.SlotWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_RefreshWidget(targetSlot.SlotWidget);

	return true;
}

void UMounteaAdvancedInventoryItemsGridWidget::ClearAllSlots_Implementation()
{
	for (auto& gridSlot : GridSlots)
	{
		const FGuid occupiedItemId = gridSlot.OccupiedItemId;
		gridSlot.OccupiedItemId = FGuid();
		
		const TObjectPtr<UUserWidget>& slotWidget = gridSlot.SlotWidget;
		if (!IsValid(slotWidget)) continue;
		if (!slotWidget->Implements<UMounteaAdvancedInventoryItemSlotWidgetInterface>()) continue;
		
		IMounteaAdvancedInventoryItemSlotWidgetInterface::Execute_StoreGridSlotData(slotWidget, gridSlot);
	}
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

int32 UMounteaAdvancedInventoryItemsGridWidget::FindEmptySlotIndex_Implementation() const
{
	return UMounteaInventoryUIStatics::Helper_FindEmptyGridSlotIndex(this);
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
