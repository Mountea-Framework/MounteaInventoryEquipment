// Copyright Dominik Morse 2023. All Rights Reserved.


#include "WBP/Inventory/MounteaInventoryItemBaseWidget.h"

#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Interfaces/UI/MounteaInventoryWBPInterface.h"

#include "Settings/MounteaInventoryThemeConfig.h"

void UMounteaInventoryItemBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsDirty = 0;
}

TScriptInterface<IMounteaInventoryWBPInterface> UMounteaInventoryItemBaseWidget::GetOwningInventory_Implementation() const
{
	return OwningInventory;
}

void UMounteaInventoryItemBaseWidget::SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryWBPInterface>& NewOwningInventory)
{
	if (OwningInventory == NewOwningInventory) return;

	OwningInventory = NewOwningInventory;
}

bool UMounteaInventoryItemBaseWidget::IsDirty_Implementation() const
{
	return bIsDirty;
}

void UMounteaInventoryItemBaseWidget::SetDirty_Implementation(const bool NewDirtyState)
{
	bIsDirty = NewDirtyState;
}

FItemSlot UMounteaInventoryItemBaseWidget::GetItem_Implementation() const
{
	FItemSlot Result;

	if (ItemStack.IsValid() == false)
	{
		return Result;
	}

	if (OwningInventory.GetObject() == nullptr)
	{
		return Result;
	}

	auto Inventory = OwningInventory->Execute_GetOwningInventory(OwningInventory.GetObject());

	if (Inventory.GetObject() == nullptr)
	{
		return Result;
	}

	FItemRetrievalFilter Filter;
	{
		Filter.bSearchByGUID = true;
		Filter.Guid = ItemStack.SlotGuid;
	}
	
	return Inventory->Execute_SearchSingleItem(Inventory.GetObject(), Filter);
}

UMounteaBaseUserWidget* UMounteaInventoryItemBaseWidget::GetItemUI_Implementation()
{
	return this;
}

FIntPoint UMounteaInventoryItemBaseWidget::GetRootCoords_Implementation() const
{
	return SlotCoordinates;
}

FIntPoint UMounteaInventoryItemBaseWidget::GetItemDimensions_Implementation() const
{
	return SlotDimensions;
}

bool UMounteaInventoryItemBaseWidget::SaveRootCoords_Implementation(const FIntPoint& NewCoords)
{
	if (NewCoords == SlotCoordinates) return false;

	SlotCoordinates = NewCoords;

	return true;
}

bool UMounteaInventoryItemBaseWidget::MoveToNewCoords_Implementation(const FIntPoint& NewCoords)
{
	if (NewCoords == SlotCoordinates) return false;

	// Do not save Root Coords yet, just broadcast and wait for container to update root
	OnItemWidgetMoved.Broadcast(this, SlotCoordinates);

	return true;
}

bool UMounteaInventoryItemBaseWidget::ReleaseOldCoords_Implementation(const FIntPoint& OldCoords)
{
	PostItemWidgetMoved.Broadcast(this, SlotCoordinates);
	
	return true;
}

TArray<FIntPoint> UMounteaInventoryItemBaseWidget::GetOccupyingCoords_Implementation() const
{
	TArray<FIntPoint> Result;
	Result.Reserve(SlotDimensions.X * SlotDimensions.Y);

	for (int X = 0; X < SlotDimensions.X; ++X)
	{
		for (int Y = 0; Y < SlotDimensions.Y; ++Y)
		{
			// Calculate the current slot based on root and offset
			FIntPoint CurrentSlot = SlotCoordinates + FIntPoint(X, Y);
			Result.Add(CurrentSlot);
		}
	}

	return Result;
}

bool UMounteaInventoryItemBaseWidget::CanBeDragged_Implementation() const
{
	const UMounteaInventoryEquipmentSettings* Settings = UMounteaInventoryEquipmentBFL::GetSettings();
	if (Settings == nullptr) return false;

	const auto InventoryUI = Execute_GetOwningInventory(this);
	if (InventoryUI.GetObject() == nullptr) return false;

	bool bThemeFound;
	const UMounteaInventoryThemeConfig* Theme = InventoryUI->Execute_GetTheme(InventoryUI.GetObject(), bThemeFound);

	if (Theme == nullptr) return false;

	const UInventoryTheme* InventoryTheme = Theme->GetInventoryTheme();
	if (InventoryTheme == nullptr) return false;

	return InventoryTheme->bDragDropAllowed;
}

void UMounteaInventoryItemBaseWidget::OnDropped_Implementation()
{
	Execute_SetDirty(this, true);
}

void UMounteaInventoryItemBaseWidget::OnDragged_Implementation()
{
}
